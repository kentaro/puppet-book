desc 'Default task (book)'
task default: :book

desc 'Create a mobi/epub-formatted file'
task book: [:mobi, :epub]

desc 'Create an html-formatted file'
file html: Dir.glob('ja/*.md') do |t|
  sh "bundle exec bin/export_html.rb #{t.prerequisites.join(' ')} > puppet-book.html"
end

desc 'Create a mobi-formatted file'
task mobi: %w[html] do
  sh "ebook-convert puppet-book.html puppet-book.mobi"
end

desc 'Create an epub-formatted file'
task epub: %w[html] do
  sh "ebook-convert puppet-book.html puppet-book.epub --no-default-epub-cover"
end

desc 'Clean out generated files'
task :clean do
  sh "rm -f puppet-book.*"
end
