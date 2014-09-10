include_directories(../include)

# objs
set(objs_corpus
./corpus_preprocess
./pfr_preprocess
./hit_preprocess
)

add_library(corpus ${objs_corpus})
