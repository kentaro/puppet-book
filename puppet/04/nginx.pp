yumrepo { 'nginx':
  name     => 'nginx',
  descr    => 'nginx yum repository',
  baseurl  => 'http://nginx.org/packages/centos/6/$basearch/',
  enabled  => 1,
  gpgcheck => 0
}

package { 'nginx':
  ensure  => installed,
  require => Yumrepo['nginx'],
}

$port = 8000

file { '/etc/nginx/conf.d/my.conf':
  ensure  => present,
  owner   => 'root',
  group   => 'root',
  mode    => '0644',
  content => template('my.conf'),
  require => Package['nginx'],
  notify  => Service['nginx'],
}

$target = 'Puppet'

file { '/usr/share/nginx/html/index.html':
  ensure  => present,
  owner   => 'root',
  group   => 'root',
  mode    => '0644',
  content => template('index.html'),
  require => Package['nginx'],
}

service { 'nginx':
  enable  => true,
  ensure  => running,
  require => File['/etc/nginx/conf.d/my.conf'],
}
