user { 'antipop':
  ensure     => present,
  gid        => 'guest',
  comment    => 'antipop',
  home       => '/home/antipop',
  managehome => true,
  shell      => '/bin/bash',
}

group { 'guest':
  ensure => present,
  gid    => 1000,
}
