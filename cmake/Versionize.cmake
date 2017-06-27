# This is called as a script at buid time (e.g. by visual studio) to get version information
# of the working directory (given as parameter WORKING_DIR) from version control system (SVN).
# It then creates Version.h from Version.h.cmake (parameter VERSION_H) with the version information.

# debug
message("Working directory: ${WORKING_DIR}")
#message("Version.h template: ${VERSION_H}")

# init variables
set(LAST_CHANGED_DATE "unknown")
set(REVISION "unknown")


# get revision from svn
#find_package(Subversion)
#if(Subversion_FOUND)
	# find svnversion
#	find_program(SVNVERSION_EXECUTABLE svnversion)
	#message(${SVNVERSION_EXECUTABLE})

	# get info
	#Subversion_WC_INFO(${WORKING_DIR} SVN)
	#set(LAST_CHANGED_DATE ${SVN_WC_LAST_CHANGED_DATE})

	# get version (indicates modification e.g. "250M")
#	execute_process(COMMAND ${SVNVERSION_EXECUTABLE} -n ${WORKING_DIR}
#		OUTPUT_VARIABLE REVISION)
#endif(Subversion_FOUND)

# get revision from git
#execute_process(COMMAND git describe WORKING_DIRECTORY ${WORKING_DIR}
#	OUTPUT_VARIABLE REVISION)

# fixed revision
set(REVISION "1.0")

#message("date = ${LAST_CHANGED_DATE}")
message("revision = ${REVISION}")


# configure Version.h file
configure_file(${VERSION_H} ${WORKING_DIR}/Version.h)
