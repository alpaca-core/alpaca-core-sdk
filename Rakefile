# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# on Windows doxygen refuses to generate multiple subdirectories (as mkdir -p)
# so we need to prepare the root doc directory manually
task :prepare_doc_subdir do
  FileUtils.mkdir_p 'out/doc'
end

task generate_cpp_doc: :prepare_doc_subdir do
  sh 'doxygen'
end

task generate_c_doc: :prepare_doc_subdir do
  sh 'doxygen wrapper/c/Doxyfile.c-api'
end

task generate_java_doc: :prepare_doc_subdir do
  sh 'doxygen wrapper/java/Doxyfile.java-api'
end

desc 'Generate all documentation'
task generate_doc: %i[generate_cpp_doc generate_c_doc generate_java_doc]
