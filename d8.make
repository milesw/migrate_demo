core = 8.x
api = 2

; Core
projects[drupal][version] = "8.0.5"

; Modules
projects[migrate_plus][version] = "1.0"
projects[migrate_tools][version] = "1.0"

; Demo module
projects[migrate_demo][download][type] = "file"
projects[migrate_demo][download][url] = "https://github.com/milesw/migrate_demo/archive/master.zip"
projects[migrate_demo][download][subtree] = "migrate_demo-master/migrate_demo_d8"
projects[migrate_demo][type] = "module"

