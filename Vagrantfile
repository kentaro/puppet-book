# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box      = "centos-6.4"
  config.vm.box_url  = "http://developer.nrel.gov/downloads/vagrant-boxes/CentOS-6.4-x86_64-v20130309.box"
  config.vm.hostname = "puppet-book.local"
  config.vm.network :forwarded_port, guest: 80, host: 8000
end
