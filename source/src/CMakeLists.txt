# objs
set(objs_cma
./analyzer
./knowledge
./sentence
./cma_factory
./cma_crf_factory
./crf_analyzer
./crf_knowledge
)

add_library(cma ${objs_cma})
