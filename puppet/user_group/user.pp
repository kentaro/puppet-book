user { 'kentaro':
  ensure     => present,
  comment    => 'kentaro',
  home       => '/home/kentaro',
  managehome => true,
  shell      => '/bin/bash',
}
