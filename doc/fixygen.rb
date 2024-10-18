# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
require 'nokogiri'

# the hackiest of solutions
#
# take a doxygen html file and tweak its bronze-age html to something usable in the 21st century
# now, the result is not usable on its own as the resulting html is not compatible with doxygen's css
# an alternative css file is needed to make it look like something
#
# the proper solution here would've been to parse the doxygen xml output and generate the html
# (or whatever else you like) from scratch, but ain't nobody got time for that sh!t

module Fixygen
  extend self

  def fix_doc(doc)
    # doxygen uses tables to layout almost everything
    # tables are messy and hard to style properly, so we'll just convert them to plain divs
    ['table', 'tr', 'td'].each do |elem|
      doc.css(elem).each do |e|
        new_elem = Nokogiri::XML::Node.new('div', doc)

        # only preserve class and id attributes
        # we don't care about the rest
        # moreover they are absolutely useless (align, valign, colspan, rowspan, etc.)
        ['class', 'id'].each do |attr|
          new_elem[attr] = e[attr] if e[attr]
        end

        # preserve what used to be there in some way
        new_elem.add_class("fix-#{elem}")

        new_elem.inner_html = e.inner_html
        e.replace(new_elem)
      end
    end

    # doxygen also uses <br> to enforse its idea of word wrapping
    # no, thank you
    doc.css('br').each(&:remove)

    # doxygen uses &nbsp; to hack its way around alignments
    # just nuke them all
    doc.css('body').each do |e|
      e.inner_html = e.inner_html.gsub("\u00A0", '')
    end

    doc
  end

  def fix_html(html) = fix_doc(Nokogiri::HTML(html)).to_html

  def fix_file(input, output = nil)
    output ||= input.sub(/\.html$/, '-fixed.html')
    html = File.read(input)
    fixed = fix_html(html)
    File.write(output, fixed)
  end
end

if $0 == __FILE__
  INPUT = ARGV[0] || 'C:/prj/alpaca-core/ac-local/out/doc/html/group__c-dict.html'
  OUTPUT = ARGV[1] || 'C:/prj/alpaca-core/ac-local/out/doc/html/group__c-dict-fixed.html'

  Fixygen.fix_file(INPUT, OUTPUT)
end
