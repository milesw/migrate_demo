<?php
/**
 * @file
 * Drush integration for the Migrate Demo module.
 */

/**
 * Implements hook_drush_command().
 */
function migrate_demo_drush_command() {
  $items = array();

  $items['generate-countries'] = array(
    'description' => 'Generate taxonomy terms for countries.',
    'drupal dependencies' => array('migrate_demo'),
    'callback' => 'migrate_demo_generate_countries',
  );
  $items['generate-organisations'] = array(
    'description' => 'Generate sample organisation nodes.',
    'drupal dependencies' => array('migrate_demo'),
    'callback' => 'migrate_demo_generate_organisations',
  );
  $items['generate-users'] = array(
    'description' => 'Generate sample users.',
    'drupal dependencies' => array('migrate_demo'),
    'callback' => 'migrate_demo_generate_users',
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function migrate_demo_drush_help($section) {
  switch ($section) {
    case 'drush:generate-countries':
      return 'Generate taxonomy terms for countries.';
    case 'drush:generate-organisations':
      return 'Generate sample organisation nodes.';
    case 'drush:generate-users':
      return 'Generate sample users.';
  }
}

/**
 * Drush command callback.
 */
function migrate_demo_generate_users() {
  $response = drupal_http_request('https://randomuser.me/api/?results=100');

  if ($response->code != 200) {
    return 'Problem requesting sample profile data from randomuser.me.';
  }

  // Wipe out all existing users except anonymous and admin.
  $users = entity_load('user');
  if (count($users) > 2) {
    unset($users[0]);
    unset($users[1]);
    user_delete_multiple(array_keys($users));
  }

  // Collect countries to randomly select from.
  // $country_tids = array();
  // $vocabulary = taxonomy_vocabulary_machine_name_load('countries');
  // foreach (taxonomy_get_tree($vocabulary->vid) as $term) {
  //   $country_tids[] = $term->tid;
  // }

  $json = json_decode($response->data);

  // Create one user account for each sample profile.
  foreach ($json->results as $result) {
    $data = $result->user;
    $user = new stdClass();
    $user->name = $data->username;
    $user->pass = $data->password;
    $user->mail = $data->email;
    $user->status = 1;

    $user->field_given_name[LANGUAGE_NONE][0]['value'] = ucwords($data->name->first);
    $user->field_family_name[LANGUAGE_NONE][0]['value'] = ucwords($data->name->last);

    // $user->field_country[LANGUAGE_NONE][0]['tid'] = $country_tids[array_rand($country_tids)];

    $file = system_retrieve_file($data->picture->large, 'public://pictures', TRUE, FILE_EXISTS_RENAME);
    $user->picture = $file->fid;

    user_save($user);
  }

  $total = count($json->results);

  return "Saved {$total} user accounts.";
}

/**
 * Drush command callback.
 */
function migrate_demo_generate_organisations() {
  // Wipe out all existing nodes.
  $nodes = entity_load('node');
  node_delete_multiple(array_keys($nodes));

  $organisations = json_decode(file_get_contents(__DIR__ . '/data/organisations.json'));

  // Get all user accounts to randomly select from.
  $uids = db_select('users', 'u')->fields('u', array('uid'))->condition('uid', 1, '>')->execute()->fetchAllKeyed(0, 0);

  // Get all country terms to randomly select from;
  $vocabulary = taxonomy_vocabulary_machine_name_load('countries');
  $tids = db_select('taxonomy_term_data', 't')->fields('t', array('tid'))->condition('vid', $vocabulary->vid)->execute()->fetchAllKeyed(0, 0);

  // Create one user account for each sample profile.
  foreach ($organisations as $organisation) {

    // Don't create nodes with duplicate titles, even though they may be
    // different records in this sample data.
    $exists = db_query('SELECT nid FROM {node} WHERE title = :title', array(':title' => $organisation->name))->fetchField();
    if ($exists) {
      continue;
    }

    $node = new stdClass();
    $node->title = $organisation->name;
    $node->type = 'organisation';
    $node->status = 1;

    $node->uid = $uids[array_rand($uids)];

    if ($organisation->description) {
      $node->body[LANGUAGE_NONE][0]['value'] = $organisation->description;
      $node->body[LANGUAGE_NONE][0]['format'] = 'plain_text';
    }

    if ($organisation->url) {
      $node->field_website[LANGUAGE_NONE][0]['url'] = $organisation->url;
    }

    // Randomly assign users as members.
    $random_uids = array_rand($uids, rand(2, 12));
    foreach ($random_uids as $uid) {
      $node->field_members[LANGUAGE_NONE][] = array('target_id' => $uid);
    }

    // Randomly assign countries.
    $random_tids = array_rand($tids, rand(2, 8));
    foreach ($random_tids as $tid) {
      $node->field_countries[LANGUAGE_NONE][] = array('target_id' => $tid);
    }

    node_save($node);
  }

  $total = count($organisations);

  return "Saved {$total} organisation nodes.";
}

/**
 * Drush command callback.
 */
function migrate_demo_generate_countries() {
  $vocabulary = taxonomy_vocabulary_machine_name_load('countries');

  // Clear all existing terms first.
  foreach (taxonomy_get_tree($vocabulary->vid) as $term) {
    taxonomy_term_delete($term->tid);
  }

  $countries = json_decode(file_get_contents('https://raw.githubusercontent.com/mledoze/countries/master/dist/countries.json'));

  $items = $codes = array();

  // First pass: Create a term for each country.
  foreach ($countries as $country) {
    $term = (object) array(
      'name' => $country->name->common,
      'vid' => $vocabulary->vid,
    );
    taxonomy_term_save($term);

    $items[] = array('country' => $country, 'term' => $term);
    $codes[$country->cca3] = $term;
  }

  // Second pass: Find and add bordering countries.
  foreach ($items as $item) {
    $term = $item['term'];
    $country = $item['country'];

    foreach ($country->borders as $code) {
      $bordering_country_term = $codes[$code];
      $term->field_bordering_countries[LANGUAGE_NONE][] = array('tid' => $bordering_country_term->tid);
    }

    if (!empty($term->field_bordering_countries)) {
      taxonomy_term_save($term);
    }
  }

  $total = count($items);

  return "Saved {$total} taxonomy terms.";
}

// https://en.wikipedia.org/wiki/List_of_fictional_Microsoft_companies
$companies = array(
  "A. Datum Corporation",
  "AdventureWorks Cycles",
  "Alpine Ski House",
  "Awesome Computers",
  "Baldwin Museum of Science",
  "Blue Yonder Airlines",
  "City Power & Light",
  "Coho Vineyard",
  "Coho Winery",
  "Coho Vineyard & Winery",
  "Consolidated Messenger",
  "Contoso Ltd.",
  "Contoso Bank",
  "Contoso Hotel (no longer owned by Microsoft)",
  "Contoso Pharmaceuticals",
  "Electronic, Inc.",
  "Fabrikam, Inc.",
  "Fourth Coffee",
  "FusionTomo",
  "Graphic Design Institute",
  "Humongous Insurance",
  "Itexamworld.com",
  "LitWare Inc.",
  "Lucerne Publishing",
  "Margie's Travel",
  "Northridge Video",
  "Northwind Traders",
  "Parnell Aerospace",
  "ProseWare, Inc.",
  "School of Fine Art",
  "Southbridge Video",
  "StrikeStrike",
  "TailSpin Toys",
  "Tasmanian Traders",
  "The Phone Company",
  "Trey Research Inc.",
  "WingTip Toys",
  "Wide World Importers",
  "Woodgrove Bank"
);