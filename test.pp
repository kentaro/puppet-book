package { 'supervisor':
  ensure   => installed,
  source   => '/tmp/supervisor-3.0a12-2.el6.noarch.rpm',
  provider => 'rpm'
}
