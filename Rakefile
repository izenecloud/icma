# Build izenelib
BRANCH = File.read(File.join(File.dirname(__FILE__), ".git/HEAD")).sub(/^.*\//, "").chomp

[
 File.join(File.dirname(__FILE__), "../cmake/lib"), # in same top directory
 File.join(File.dirname(__FILE__), "../../cmake--master/workspace/lib") # for hudson
].each do |dir|
  if File.directory? dir
    $: << dir
    ENV["EXTRA_CMAKE_MODULES_DIRS"] = File.dirname(File.expand_path(dir))
  end
end

require "izenesoft/tasks"

task :default => :cmake

IZENESOFT::CMake.new do |t|
  t.build_type = "release" # default type is release
  t["CMAKE_CMA_DEBUG_PRINT"] = 0
end

IZENESOFT::GITClean.new
