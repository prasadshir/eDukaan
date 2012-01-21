<?php

/**
 * @file
 * Provides a central controller for Drupal Commerce.
 *
 * A full fork of Entity API's controller, with support for revisions.
 */

class DrupalCommerceEntityController extends DrupalDefaultEntityController implements EntityAPIControllerInterface {

  /**
   * Stores our transaction object, necessary for pessimistic locking to work.
   */
  protected $controllerTransaction = NULL;

  /**
   * Override of DrupalDefaultEntityController::buildQuery().
   *
   * Handle pessimistic locking.
   */
  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    $query = parent::buildQuery($ids, $conditions, $revision_id);
    if (isset($this->entityInfo['locking mode']) && $this->entityInfo['locking mode'] == 'pessimistic') {
      // In pessimistic locking mode, we issue the load query with a FOR UPDATE
      // clause. This will block all other load queries to the loaded objects
      // but requires us to start a transaction.
      if (empty($this->controllerTransaction)) {
        $this->controllerTransaction = db_transaction();
      }

      $query->forUpdate();
    }

    return $query;
  }

  public function resetCache(array $ids = NULL) {
    parent::resetCache($ids);
    if (empty($this->entityCache) && !empty($this->controllerTransaction)) {
      // If we don't have any entity in our local cache anymore, we commit the
      // transaction so as to remove the locks we acquired.
      // This will not commit the translation directly. Drupal will commit
      // it as soon as possible given the state of the transaction stack.
      unset($this->controllerTransaction);
    }
  }

  /**
   * (Internal use) Invokes a hook on behalf of the entity.
   *
   * For hooks that have a respective field API attacher like insert/update/..
   * the attacher is called too.
   */
  public function invoke($hook, $entity) {
    if (!empty($this->entityInfo['fieldable']) && function_exists($function = 'field_attach_' . $hook)) {
      $function($this->entityType, $entity);
    }

    // Invoke the hook. If rules is there, use the rule funtion so that a rules
    // event is invoked too.
    if (module_exists('rules')) {
      rules_invoke_all($this->entityType . '_' . $hook, $entity);
    }
    else {
      module_invoke_all($this->entityType . '_' . $hook, $entity);
    }
    // Invoke the respective entity level hook.
    if ($hook == 'presave' || $hook == 'insert' || $hook == 'update' || $hook == 'delete') {
      module_invoke_all('entity_' . $hook, $entity, $this->entityType);
    }
  }

  /**
   * Delete permanently saved entities.
   *
   * In case of failures, an exception is thrown.
   *
   * @param $ids
   *   An array of entity IDs.
   * @param $transaction
   *   An optional transaction object to pass thru. If passed the caller is
   *   responsible for rolling back the transaction if something goes wrong.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $entities = $ids ? $this->load($ids) : FALSE;
    if (!$entities) {
      // Do nothing, in case invalid or no ids have been passed.
      return;
    }

    if (!isset($transaction)) {
      $transaction = db_transaction();
      $started_transaction = TRUE;
    }

    try {
      db_delete($this->entityInfo['base table'])
        ->condition($this->idKey, array_keys($entities), 'IN')
        ->execute();
      if (!empty($this->revisionKey)) {
        db_delete($this->entityInfo['revision table'])
          ->condition($this->idKey, array_keys($entities), 'IN')
          ->execute();
      }
      // Reset the cache as soon as the changes have been applied.
      $this->resetCache($ids);

      foreach ($entities as $id => $entity) {
        $this->invoke('delete', $entity);
      }
      // Ignore slave server temporarily.
      db_ignore_slave();

      return TRUE;
    }
    catch (Exception $e) {
      if (!empty($started_transaction)) {
        $transaction->rollback();
        watchdog_exception($this->entityType, $e);
      }
      throw $e;
    }
  }

  /**
   * Permanently saves the given entity.
   *
   * In case of failures, an exception is thrown.
   *
   * @param $entity
   *   The entity to save.
   * @param $transaction
   *   An optional transaction object to pass thru. If passed the caller is
   *   responsible for rolling back the transaction if something goes wrong.
   *
   * @return
   *   SAVED_NEW or SAVED_UPDATED depending on the operation performed.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    if (!isset($transaction)) {
      $transaction = db_transaction();
      $started_transaction = TRUE;
    }

    try {
      // Load the stored entity, if any.
      if (!empty($entity->{$this->idKey}) && !isset($entity->original)) {
        // In order to properly work in case of name changes, load the original
        // entity using the id key if it is available.
        $entity->original = entity_load_unchanged($this->entityType, $entity->{$this->idKey});
      }

      $this->invoke('presave', $entity);

      // When saving a new revision, unset any existing revision ID so as to
      // ensure that a new revision will actually be created, then store the old
      // revision ID in a separate property for use by hook implementations.
      if (!empty($this->revisionKey) && empty($entity->is_new) && !empty($entity->revision) && !empty($entity->{$this->revisionKey})) {
        $entity->old_revision_id = $entity->{$this->revisionKey};
        unset($entity->{$this->revisionKey});
      }

      if (empty($entity->{$this->idKey}) || !empty($entity->is_new)) {
        // For new entities, create the row in the base table, then save the
        // revision.
        $op = 'insert';
        $return = drupal_write_record($this->entityInfo['base table'], $entity);
        if (!empty($this->revisionKey)) {
          drupal_write_record($this->entityInfo['revision table'], $entity);
          $update_base_table = TRUE;
        }
      }
      else {
        $op = 'update';
        $return = drupal_write_record($this->entityInfo['base table'], $entity, $this->idKey);

        if (!empty($this->revisionKey)) {
          if (!empty($entity->revision)) {
            drupal_write_record($this->entityInfo['revision table'], $entity);
            $update_base_table = TRUE;
          }
          else {
            drupal_write_record($this->entityInfo['revision table'], $entity, $this->revisionKey);
          }
        }
      }

      if (!empty($update_base_table)) {
        // Go back to the base table and update the pointer to the revision ID.
        db_update($this->entityInfo['base table'])
        ->fields(array($this->revisionKey => $entity->{$this->revisionKey}))
        ->condition($this->idKey, $entity->{$this->idKey})
        ->execute();
      }

      $this->resetCache(array($entity->{$this->idKey}));
      $this->invoke($op, $entity);

      // Ignore slave server temporarily.
      db_ignore_slave();
      unset($entity->is_new);
      unset($entity->original);
      unset($entity->revision);

      return $return;
    }
    catch (Exception $e) {
      if (!empty($started_transaction)) {
        $transaction->rollback();
        watchdog_exception($this->entityType, $e);
      }
      throw $e;
    }
  }

  /**
   * Create a new entity.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   * @return
   *   A new instance of the entity type.
   */
  public function create(array $values = array()) {
    // Add is_new property if it is not set.
    $values += array('is_new' => TRUE);

    // If there is a class for this entity type, instantiate it now.
    if (isset($this->entityInfo['entity class']) && $class = $this->entityInfo['entity class']) {
      $entity = new $class($values, $this->entityType);
    }
    else {
      // Otherwise use a good old stdClass.
      $entity = (object) $values;
    }

    // Allow other modules to alter the created entity.
    drupal_alter('commerce_entity_create', $this->entityType, $entity);

    return $entity;
  }

  /**
   * Implements EntityAPIControllerInterface.
   */
  public function export($entity, $prefix = '') {
    throw new Exception('Not implemented');
  }

  /**
   * Implements EntityAPIControllerInterface.
   */
  public function import($export) {
    throw new Exception('Not implemented');
  }

  /**
   * Builds a structured array representing the entity's content.
   *
   * The content built for the entity will vary depending on the $view_mode
   * parameter.
   *
   * @param $entity
   *   An entity object.
   * @param $view_mode
   *   View mode, e.g. 'full', 'teaser'...
   * @param $langcode
   *   (optional) A language code to use for rendering. Defaults to the global
   *   content language of the current request.
   * @return
   *   The renderable array.
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    // Remove previously built content, if exists.
    $entity->content = $content;
    $langcode = isset($langcode) ? $langcode : $GLOBALS['language_content']->language;

    // Add in fields.
    if (!empty($this->entityInfo['fieldable'])) {
      $entity->content += field_attach_view($this->entityType, $entity, $view_mode, $langcode);
    }

    // Invoke hook_ENTITY_view() to allow modules to add their additions.
    rules_invoke_all($this->entityType . '_view', $entity, $view_mode, $langcode);

    // Invoke the more generic hook_entity_view() to allow the same.
    module_invoke_all('entity_view', $entity, $this->entityType, $view_mode, $langcode);

    // Remove the build array information from the entity and return it.
    $build = $entity->content;
    unset($entity->content);

    return $build;
  }

  /**
   * Generate an array for rendering the given entities.
   *
   * @param $entities
   *   An array of entities to render.
   * @param $view_mode
   *   View mode, e.g. 'full', 'teaser'...
   * @param $langcode
   *   (optional) A language code to use for rendering. Defaults to the global
   *   content language of the current request.
   * @param $page
   *   (optional) If set will control if the entity is rendered: if TRUE
   *   the entity will be rendered without its title, so that it can be embeded
   *   in another context. If FALSE the entity will be displayed with its title
   *   in a mode suitable for lists.
   *   If unset, the page mode will be enabled if the current path is the URI
   *   of the entity, as returned by entity_uri().
   *   This parameter is only supported for entities which controller is a
   *   EntityAPIControllerInterface.
   * @return
   *   The renderable array.
   */
  public function view($entities, $view_mode = '', $langcode = NULL, $page = NULL) {
    // Create a new entities array keyed by entity ID.
    $rekeyed_entities = array();

    foreach ($entities as $key => $entity) {
      // Use the entity's ID if available and fallback to its existing key value
      // if we couldn't determine it.
      if (isset($entity->{$this->idKey})) {
        $key = $entity->{$this->idKey};
      }

      $rekeyed_entities[$key] = $entity;
    }

    $entities = $rekeyed_entities;

    // If no view mode is specified, use the first one available..
    if (!isset($this->entityInfo['view modes'][$view_mode])) {
      reset($this->entityInfo['view modes']);
      $view_mode = key($this->entityInfo['view modes']);
    }

    if (!empty($this->entityInfo['fieldable'])) {
      field_attach_prepare_view($this->entityType, $entities, $view_mode);
    }

    entity_prepare_view($this->entityType, $entities);
    $langcode = isset($langcode) ? $langcode : $GLOBALS['language_content']->language;
    $view = array();

    // Build the content array for each entity passed in.
    foreach ($entities as $key => $entity) {
      $build = entity_build_content($this->entityType, $entity, $view_mode, $langcode);

      // Add default properties to the array to ensure the content is passed
      // through the theme layer.
      $build += array(
        '#theme' => 'entity',
        '#entity_type' => $this->entityType,
        '#entity' => $entity,
        '#view_mode' => $view_mode,
        '#language' => $langcode,
        '#page' => $page,
      );

      // Allow modules to modify the structured entity.
      drupal_alter(array($this->entityType . '_view', 'entity_view'), $build, $this->entityType);
      $view[$this->entityType][$key] = $build;
    }

    return $view;
  }

}