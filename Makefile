#--------------------------------------------------------------------------
# следующие 3 строки лучше не трогать
ifeq (,$(WORKDIR))
WORKDIR=./..
endif

# Всё что делается делается вслух :) - желательно оставить как есть
VERBOSEMAKE=""
# Если ключик -g при сборки не нужен - если уверен в себе
NDEBUG=""

# Компилятор для *.с файлов
#CC_LOCAL := gcc
# Компилятор для *.cc и *.cpp файлов
#CXX_LOCAL := g++
# Компилятор для *.f, *.for и *.F файлов
#F77_LOCAL := g77
# Линкер для этого пакета по умолчанию
#LD_LOCAL := g++
# Раскомментировать, если необходимо задавать линкер для каждого
# бинарника в отдельности. Обычно не требуется.
#LD_MULTI := ""

# Дополнительные опции для компилятора C/C++
#NOOPT = ""
COPTS  = `root-config --cflags`

# Дополнительные опции для компилятора Fortran
FOPTS =

# Дополнительные опции для линкера
#OOTLIBDIR := $(shell root-config --libdir)
#DOPTS = -Xlinker -rpath -Xlinker $(ROOTLIBDIR)
#OCYCLE = ""
LDOPTS = -lg2c `root-config --libs --glibs` -lGui -lMinuit -lNew -Wl,-rpath -Wl,`root-config --libdir`

#Если определена переменная ONLYBINARY, то библиотека в пакете отсутствует
ONLYBINARY = ""

# Если необходимо добавить CERNLIB, то лучше воспользоваться этим
# ключиком. Церновские библиотеки подшиваются в самом конце. По
# умалчанию подшиваются следущие библиотеки jetset74 mathlib graflib
# geant321 grafX11 packlib
CERNLIBRARY = ""

# Список библиотек, если вам не нравится стандартный набор, который
# расширяем по требованию. В любом случае необходимо раскомментировать
#CERNLIBS = jetset74 mathlib graflib geant lapack3 blas
CERNLIBS =
# где вываливать исполняемые файлы
#BINDIR := ./

# Дополнительные либы (вставляются после либ)
#LIB_LOCAL= $(shell root-config --libs) -lMinuit -lpq -lcrypt -lbz2 -ldl -lg2c -lm -lstdc++
#LIB_LOCAL= -lpq -lcrypt -lbz2 -lg2c -ldl -lstdc++
LIB_LOCAL=-lbz2 -lg2c -lcrypt -lpq

# Определим, какие программы мы будем собирать
BINARIES = check_run time_run

# укажем, из каких модулей этого пакета они состоят
# (эти модули не будут включены в библиотеку)
# и какие библиотеки надо подключить при сборке
check_run_MODULES = check_run
check_run_LIBS = KDB Common

time_run_MODULES = time_run
time_run_LIBS = KDB Common

#atc_read_runsmonitor_MODULES = atc_read_runsmonitor
#atc_read_runsmonitor_LIBS = KDB Common

# следующую строку лучше не трогать
include $(WORKDIR)/KcReleaseTools/rules.mk

