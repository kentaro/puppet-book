desc 'Default task (book)'
task default: :book

desc 'Create a mobi/epub-formatted file'
task book: %i[mobi epub]

desc 'Create an html-formatted file'
file html: Dir.glob('ja/*.md') do |t|
  sh "bundle exec bin/export_html.rb #{t.prerequisites.join(' ')} > puppet.html"
end

desc 'Create a mobi-formatted file'
task mobi: %w[html] do
  sh "ebook-convert puppet.html puppet.mobi"
end

desc 'Create an epub-formatted file'
task epub: %w[html] do
  sh "ebook-convert puppet.html puppet.epub --no-default-epub-cover"
end

desc 'Clean out generated files'
task :clean do
  sh "rm -f puppet.html puppet.mobi puppet.epub"
end
