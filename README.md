# Intro to Migrate in Drupal 8

This is a demo prepared for the Bangkok Drupal Meetup on March 31, 2016. It demonstrates a basic Drupal 7 to Drupal 8 content migration using the Migrate module and Drush. Content being migrated includes: organisations (nodes), countries (taxonony terms), and people (users). Drush scripts to populate sample content are included. 

Slides: https://docs.google.com/presentation/d/1O5UpI0pJj7Yi1nhj8ZtEzbZzzAk1qUso7-n4uggenp0/pub?start=false&loop=false&delayms=5000

![Screenshot of demo sites](https://raw.githubusercontent.com/milesw/migrate_demo/images/screenshot.jpg)

## Drupal 7 Setup

1. drush make https://raw.githubusercontent.com/milesw/migrate_demo/master/d7.make migrate_demo_d7
2. Install Drupal 7 Standard profile
3. drush en -y migrate_demo
4. drush populate-all

## Drupal 8 Setup

1. drush make https://raw.githubusercontent.com/milesw/migrate_demo/master/d8.make migrate_demo_d8
2. Install Drupal 8 Standard profile
3. drush en -y migrate_demo
4. drush config-set "system.site" uuid "7322b825-4dc6-437e-9c00-0abfdc04ce8a"
5. Load the config.tar.gz from this repo at /admin/config/development/configuration/full/import  
   (may need to delete shortcut entities from the database?)
6. Add database connection for D7 site to D8 settings.php:
    ```
    $databases['migrate']['default'] = array (
      'database' => 'dbname',
      'username' => 'dbuser',
      'password' => 'dbpass',
      'host' => '127.0.0.1',
      'port' => '3306',
      'driver' => 'mysql',
    );
    ```
    
7. drush migrate-status (if you see totals, database connection is good)
8. drush migrate-import --all
        
