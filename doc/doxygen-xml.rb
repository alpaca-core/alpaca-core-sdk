require 'nokogiri'

XML_DIR = File.expand_path(Dir.pwd + '/../out/doc/xml')

# find groups in index.xml
groups = Nokogiri::XML(File.open(XML_DIR + '/index.xml')).root.elements.select {
  _1[:kind] == 'group'
}.map { |g|
  name = g.elements.find { _1.name == 'name' }.inner_text
  [name, g[:refid]]
}.to_h

p groups

