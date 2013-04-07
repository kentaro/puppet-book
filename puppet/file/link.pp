file { '/etc/nginx/site-available/mysite.conf':
  ensure => present,
  owner  => 'root',
  group  => 'root',
  mode   => '0644',
}

file { '/etc/nginx/site-enabled/mysite.conf':
  ensure => link,
  target => '/etc/nginx/site-available/mysite.conf',
  owner  => 'root',
  group  => 'root',
  mode   => '0644',
  require => File['/etc/nginx/site-available/mysite.conf'],
}
