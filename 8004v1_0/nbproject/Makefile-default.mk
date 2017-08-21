#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/8004v1_0.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/8004v1_0.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../AD5241a.c ../LC8004delay_b.c ../LC8004extFRAM_i.c ../lc8004main_1_0.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/AD5241a.o ${OBJECTDIR}/_ext/1472/LC8004delay_b.o ${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o ${OBJECTDIR}/_ext/1472/lc8004main_1_0.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/AD5241a.o.d ${OBJECTDIR}/_ext/1472/LC8004delay_b.o.d ${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o.d ${OBJECTDIR}/_ext/1472/lc8004main_1_0.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/AD5241a.o ${OBJECTDIR}/_ext/1472/LC8004delay_b.o ${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o ${OBJECTDIR}/_ext/1472/lc8004main_1_0.o

# Source Files
SOURCEFILES=../AD5241a.c ../LC8004delay_b.c ../LC8004extFRAM_i.c ../lc8004main_1_0.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/8004v1_0.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ256GP710A
MP_LINKER_FILE_OPTION=,--script=p33FJ256GP710A.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/AD5241a.o: ../AD5241a.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/AD5241a.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/AD5241a.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../AD5241a.c  -o ${OBJECTDIR}/_ext/1472/AD5241a.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/AD5241a.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1    -omf=elf -legacy-libc  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/AD5241a.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/LC8004delay_b.o: ../LC8004delay_b.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/LC8004delay_b.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/LC8004delay_b.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../LC8004delay_b.c  -o ${OBJECTDIR}/_ext/1472/LC8004delay_b.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/LC8004delay_b.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1    -omf=elf -legacy-libc  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LC8004delay_b.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o: ../LC8004extFRAM_i.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../LC8004extFRAM_i.c  -o ${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1    -omf=elf -legacy-libc  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/lc8004main_1_0.o: ../lc8004main_1_0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/lc8004main_1_0.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/lc8004main_1_0.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lc8004main_1_0.c  -o ${OBJECTDIR}/_ext/1472/lc8004main_1_0.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/lc8004main_1_0.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1    -omf=elf -legacy-libc  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/lc8004main_1_0.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1472/AD5241a.o: ../AD5241a.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/AD5241a.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/AD5241a.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../AD5241a.c  -o ${OBJECTDIR}/_ext/1472/AD5241a.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/AD5241a.o.d"        -g -omf=elf -legacy-libc  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/AD5241a.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/LC8004delay_b.o: ../LC8004delay_b.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/LC8004delay_b.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/LC8004delay_b.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../LC8004delay_b.c  -o ${OBJECTDIR}/_ext/1472/LC8004delay_b.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/LC8004delay_b.o.d"        -g -omf=elf -legacy-libc  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LC8004delay_b.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o: ../LC8004extFRAM_i.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../LC8004extFRAM_i.c  -o ${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o.d"        -g -omf=elf -legacy-libc  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LC8004extFRAM_i.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/lc8004main_1_0.o: ../lc8004main_1_0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/lc8004main_1_0.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/lc8004main_1_0.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lc8004main_1_0.c  -o ${OBJECTDIR}/_ext/1472/lc8004main_1_0.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/lc8004main_1_0.o.d"        -g -omf=elf -legacy-libc  -mlarge-code -mlarge-data -mlarge-scalar -mconst-in-data -O0 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/lc8004main_1_0.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/8004v1_0.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/8004v1_0.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_REAL_ICE=1  -omf=elf -legacy-libc   -mreserve=data@0x800:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x825 -mreserve=data@0x826:0x84F   -Wl,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_REAL_ICE=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/8004v1_0.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/8004v1_0.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -legacy-libc  -Wl,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/8004v1_0.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
