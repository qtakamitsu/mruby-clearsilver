MRuby::Gem::Specification.new('mruby-clearsilver') do |spec|
  spec.license = 'MIT'
  spec.author  = 'qtakamitsu'
  spec.version = '0.0.1'
 
  spec.cc.flags << '-g -I/usr/local/include/ClearSilver'
  spec.mruby.cc.flags << '-g'
  spec.linker.library_paths = '/usr/local/lib'
  spec.linker.libraries << %w(neo_cgi neo_utl neo_cs)
end
