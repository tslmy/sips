#---------------------------------------------------------------------------------------------------------------------
# TARGET is the name of the output.
# BUILD is the directory where object files & intermediate files will be placed.
# LIBBUTANO is the main directory of butano library (https://github.com/GValiente/butano).
# PYTHON is the path to the python interpreter.
# SOURCES is a list of directories containing source code.
# INCLUDES is a list of directories containing extra header files.
# DATA is a list of directories containing binary data.
# GRAPHICS is a list of directories containing files to be processed by grit.
# AUDIO is a list of directories containing files to be processed by mmutil.
# DMGAUDIO is a list of directories containing files to be processed by mod2gbt and s3m2gbt.
# ROMTITLE is a uppercase ASCII, max 12 characters text string containing the output ROM title.
# ROMCODE is a uppercase ASCII, max 4 characters text string containing the output ROM code.
# USERFLAGS is a list of additional compiler flags:
#     Pass -flto to enable link-time optimization.
#     Pass -O0 to improve debugging.
# USERASFLAGS is a list of additional assembler flags.
# USERLDFLAGS is a list of additional linker flags:
#     Pass -flto=auto -save-temps to enable parallel link-time optimization.
# USERLIBDIRS is a list of additional directories containing libraries.
#     Each libraries directory must contains include and lib subdirectories.
# USERLIBS is a list of additional libraries to link with the project.
# USERBUILD is a list of additional directories to remove when cleaning the project.
# EXTTOOL is an optional command executed before processing audio, graphics and code files.
#
# All directories are specified relative to the project directory where the makefile is found.
#---------------------------------------------------------------------------------------------------------------------
TARGET      :=  $(notdir $(CURDIR))
BUILD       :=  build
PROJECT_ROOT := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
LIBBUTANO   ?= $(PROJECT_ROOT)/external/butano/butano
LIBBUTANO_FALLBACKS := $(PROJECT_ROOT)/external/butano/butano $(PROJECT_ROOT)/../butano/butano $(PROJECT_ROOT)/.deps/butano/butano
PYTHON      :=  /opt/homebrew/Caskroom/miniforge/base/bin/python3
SOURCES     :=  src
INCLUDES    :=  include
DATA        :=
GRAPHICS    :=  graphics
AUDIO       :=  audio
DMGAUDIO    :=  dmg_audio
ROMTITLE    :=  sips
ROMCODE     :=  SBTP
USERFLAGS   :=  
USERASFLAGS :=  
USERLDFLAGS :=  
USERLIBDIRS :=  
USERLIBS    :=  
USERBUILD   :=  
EXTTOOL     :=  

#---------------------------------------------------------------------------------------------------------------------
# Resolve Butano path:
# - Use LIBBUTANO if provided and valid.
# - Otherwise, auto-discover from fallback locations.
#---------------------------------------------------------------------------------------------------------------------
LIBBUTANO_FOUND := $(firstword $(foreach path,$(LIBBUTANO) $(LIBBUTANO_FALLBACKS),$(if $(wildcard $(path)/butano.mak),$(path))))

ifeq ($(LIBBUTANO_FOUND),)
  $(error Could not locate Butano. Run `just setup-butano` (git submodule init) or set LIBBUTANO=/absolute/path/to/butano)
endif

ifndef LIBBUTANOABS
	export LIBBUTANOABS := $(realpath $(LIBBUTANO_FOUND))
endif

#---------------------------------------------------------------------------------------------------------------------
# Include main makefile:
#---------------------------------------------------------------------------------------------------------------------
include $(LIBBUTANOABS)/butano.mak
