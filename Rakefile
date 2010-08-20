# Wrapper of CMAKE
[
 ENV["EXTRA_CMAKE_MODULES_DIRS"],
 File.join(File.dirname(__FILE__), "../cmake"), # in same top directory
 File.join(File.dirname(__FILE__), "../../cmake--master/workspace") # for hudson
].each do |dir|
  next unless dir
  dir = File.expand_path(dir)
  if File.exists? File.join(dir, "Findizenelib.cmake")
    $: << File.join(dir, "lib")
    ENV["EXTRA_CMAKE_MODULES_DIRS"] = dir
  end
end

require "izenesoft/tasks"

task :default => :cmake

IZENESOFT::CMake.new do |t|
  t.build_type = "release" # default type is release
  t["CMAKE_CMA_DEBUG_PRINT"] = 0
end

IZENESOFT::GITClean.new

IZENESOFT::BoostTest.new