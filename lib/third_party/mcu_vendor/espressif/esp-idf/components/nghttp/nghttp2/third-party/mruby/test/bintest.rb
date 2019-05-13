$:.unshift File.dirname(File.dirname(File.expand_path(__FILE__)))
require 'test/assert.rb'

def cmd(s)
  case RbConfig::CONFIG['host_os']
  when /mswin(?!ce)|mingw|cygwin|bccwin/
    "bin\\#{s}.exe"
  else
    "bin/#{s}"
  end
end

def shellquote(s)
  case RbConfig::CONFIG['host_os']
  when /mswin(?!ce)|mingw|cygwin|bccwin/
    "\"#{s}\""
  else
    "'#{s}'"
  end
end

ARGV.each do |gem|
  Dir["#{gem}/bintest/**/*.rb"].each do |file|
    load file
  end
end

load 'test/report.rb'
