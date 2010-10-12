##################################################
# Makefile for sf1v 4.5
##################################################
# Author 	: Dohyun Yun
# Date 		: 2010.10.12
##################################################
# Description : This is for building static 
# 				library.
# Prerequisite
# 	- Environmental variables
# 		SF1_SRC
##################################################



##################################################
# include c-compiler environment makefile
##################################################
ifeq ($(origin SF1_SRC), undefined)
include ./Makefile.defs
else
include $(SF1_SRC)/build_system/common-defs.mak
endif

SRC_HOME = $(CURDIR)/source
LIB_OUT = lib/libcmac.a

##################################################
# include paths
##################################################

INC_DIRS = -I./include \
		   -I$(SRC_HOME) \
		   -I$(SRC_HOME)/include \
		   -I$(SRC_HOME)/corpus \
		   -I$(SRC_HOME)/maxent \
		   -I$(SRC_HOME)/vsynonym

##################################################
# objects
##################################################

OBJS = $(OBJ_SRC) $(OBJ_TIXML) $(OBJ_CORPUS) $(OBJ_MAXENT)

OBJ_SRC = $(patsubst %.cpp, %.o, $(wildcard $(SRC_HOME)/*.cpp)) \
          $(patsubst %.cc, %.o, $(wildcard $(SRC_HOME)/*.cc)) \
		  $(OBJ_SRC_DICTB) \
		  $(OBJ_SRC_FMINCOVER) \
		  $(OBJ_SRC_ME ) \
		  $(OBJ_SRC_TYPE) \
		  $(OBJ_SRC_UTIL )

OBJ_SRC_DICTB = $(patsubst %.cpp, %.o, $(wildcard $(SRC_HOME)/dictb/*.cpp))
OBJ_SRC_FMINCOVER = $(patsubst %.cpp, %.o, $(wildcard $(SRC_HOME)/fmincover/*.cpp))
OBJ_SRC_ME = $(patsubst %.cc, %.o, $(wildcard $(SRC_HOME)/me/*.cc))
OBJ_SRC_TYPE = $(patsubst %.cpp, %.o, $(wildcard $(SRC_HOME)/type/*.cpp))
OBJ_SRC_UTIL = $(patsubst %.cpp, %.o, $(wildcard $(SRC_HOME)/util/*.cpp)) \
			   $(patsubst %.cc, %.o, $(wildcard $(SRC_HOME)/util/*.cc))

OBJ_TIXML = $(patsubst %.cpp, %.o, $(wildcard $(SRC_HOME)/tixml/*.cpp))

OBJ_CORPUS = $(patsubst %.cpp, %.o, $(wildcard $(SRC_HOME)/corpus/*.cpp))

OBJ_MAXENT = $(SRC_HOME)/maxent/display.o \
			 $(SRC_HOME)/maxent/gistrainer.o \
			 $(SRC_HOME)/maxent/maxentmodel.o \
			 $(SRC_HOME)/maxent/trainer.o \
			 $(SRC_HOME)/maxent/mmapfile.o \
			 $(SRC_HOME)/maxent/modelfile.o \
			 $(SRC_HOME)/maxent/maxent_cmdline.o

##################################################
# suffixes rules
##################################################
.SUFFIXES: .cpp .cc .c .o .d

##################################################
# all
##################################################
all: $(LIB_OUT)

##################################################
# install
##################################################
#install:


##################################################
# compile
##################################################
%.o: %.cpp
	$(CC_PROG) $(CC_FLAGS) $(INC_DIRS) -o $@ $<

%.o: %.cc
	$(CC_PROG) $(CC_FLAGS) $(INC_DIRS) -o $@ $<

%.o: %.c
	$(CC_PROG) $(CC_FLAGS) $(INC_DIRS) -o $@ $<

##################################################
# library output
##################################################
$(LIB_OUT): $(OBJS)
	$(LIB_PROG) $(LIB_FLAGS) $@ $?

##################################################
# clean
##################################################
clean: clean-lib
	$(DEL_PROG) $(OBJS) $(OBJS:.o=.d)
clean-lib:
	$(DEL_PROG) $(LIB_OUT)

