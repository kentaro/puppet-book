#!/usr/bin/env ruby
# by Tatsuhiko Miyagawa

require 'github/markdown'

HEADER = <<HEAD
<html>
<head>
<title>入門Puppet - Automate Your Infrastructure</title>
<meta name="Author" content="Kentaro Kuribayashi">
<meta name="DC.date.publication" content="2013-05">
<meta name="DC.rights" content="2013 Kentaro Kuribayashi">
<link rel="stylesheet" href="css/epub.css" type="text/css" class="horizontal" title="Horizontal Layout" />
</head>
<body>
HEAD

def munge(html)
  html.gsub!(/\.\.\/images\//, 'images/')
  yield(html).gsub /<h2>/, '<h2 class="chapter">'
end

STDOUT.write HEADER
STDOUT.write munge(ARGF.readlines.join '') do |html|
  GitHub::Markdown.render_gfm(html)
end
STDOUT.write "</body></html>\n"
