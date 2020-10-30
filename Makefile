#--------------------------------------------------------------------------
# ��������� 3 ������ ����� �� �������
ifeq (,$(WORKDIR))
WORKDIR=./..
endif

# �ӣ ��� �������� �������� ����� :) - ���������� �������� ��� ����
VERBOSEMAKE=""
# ���� ������ -g ��� ������ �� ����� - ���� ������ � ����
NDEBUG=""

# ���������� ��� *.� ������
#CC_LOCAL := gcc
# ���������� ��� *.cc � *.cpp ������
#CXX_LOCAL := g++
# ���������� ��� *.f, *.for � *.F ������
#F77_LOCAL := g77
# ������ ��� ����� ������ �� ���������
#LD_LOCAL := g++
# �����������������, ���� ���������� �������� ������ ��� �������
# ��������� � �����������. ������ �� ���������.
#LD_MULTI := ""

# �������������� ����� ��� ����������� C/C++
#NOOPT = ""
COPTS  = `root-config --cflags`

# �������������� ����� ��� ����������� Fortran
FOPTS =

# �������������� ����� ��� �������
#OOTLIBDIR := $(shell root-config --libdir)
#DOPTS = -Xlinker -rpath -Xlinker $(ROOTLIBDIR)
#OCYCLE = ""
LDOPTS = -lg2c `root-config --libs --glibs` -lGui -lMinuit -lNew -Wl,-rpath -Wl,`root-config --libdir`

#���� ���������� ���������� ONLYBINARY, �� ���������� � ������ �����������
ONLYBINARY = ""

# ���� ���������� �������� CERNLIB, �� ����� ��������������� ����
# ��������. ���������� ���������� ����������� � ����� �����. ��
# ��������� ����������� �������� ���������� jetset74 mathlib graflib
# geant321 grafX11 packlib
CERNLIBRARY = ""

# ������ ���������, ���� ��� �� �������� ����������� �����, �������
# ��������� �� ����������. � ����� ������ ���������� �����������������
#CERNLIBS = jetset74 mathlib graflib geant lapack3 blas
CERNLIBS =
# ��� ���������� ����������� �����
#BINDIR := ./

# �������������� ���� (����������� ����� ���)
#LIB_LOCAL= $(shell root-config --libs) -lMinuit -lpq -lcrypt -lbz2 -ldl -lg2c -lm -lstdc++
#LIB_LOCAL= -lpq -lcrypt -lbz2 -lg2c -ldl -lstdc++
LIB_LOCAL=-lbz2 -lg2c -lcrypt -lpq

# ���������, ����� ��������� �� ����� ��������
BINARIES = check_run time_run

# ������, �� ����� ������� ����� ������ ��� �������
# (��� ������ �� ����� �������� � ����������)
# � ����� ���������� ���� ���������� ��� ������
check_run_MODULES = check_run
check_run_LIBS = KDB Common

time_run_MODULES = time_run
time_run_LIBS = KDB Common

#atc_read_runsmonitor_MODULES = atc_read_runsmonitor
#atc_read_runsmonitor_LIBS = KDB Common

# ��������� ������ ����� �� �������
include $(WORKDIR)/KcReleaseTools/rules.mk

