#!/usr/bin/env ruby
# by Tatsuhiko Miyagawa

require 'github/markdown'

HEADER = <<HEAD
<html>
<head>
<title>入門Puppet - Infrastructure as Code</title>
<meta name="Author" content="Kentaro Kuribayashi">
<meta name="DC.date.publication" content="2013-05">
<meta name="DC.rights" content="2013 Kentaro Kuribayashi">
<link rel="stylesheet" href="css/epub.css" type="text/css" class="horizontal" title="Horizontal Layout" />
</head>
<body>
HEAD

def munge(html)
  yield(html).gsub /<h1>/, '<h1 class="chapter">'
end

STDOUT.write HEADER
STDOUT.write munge(ARGF.readlines.join '') do |html|
  GitHub::Markdown.render_gfm(html)
end
STDOUT.write "</body></html>\n"
