[2010-05-26] TODO: this file needs to be updated to reflect the new GIT repository and build process.  Maybe remove
since that information is in BUILDING.TXT

==============================================
NAME:      Appscio Media Processing Framework (MPF)
VERSION:   Initial Release
CREATED BY Curt Patrick
==============================================

INDEX:

-> About this version
-> Requirements
-> Installation
-> Usage
-> Road Map
-> Version History

==============================================
ABOUT THIS VERSION:
===============================================
This is the initial release of the following MPF packages:

- grdf-0.0.2-1.fc10.x86_64.rpm
- mpf-core-0.0.4-1.fc10.x86_64.rpm
- mpf-gst-modified-0.0.2-1.fc10.x86_64.rpm
- mpf-pittpatt-ftr-0.0.2-1.fc10.x86_64.rpm
- mpf-segmenter-0.0.2-1.fc10.x86_64.rpm
- mpf-rdf-0.0.2-1.fc10.x86_64.rpm
- mpf-toi-0.0.2-1.fc10.x86_64.rpm

Sources RPMS are available for these packages at
   http://www.appscio.com/repository/packages/release/SRPMS/.
Tarballs for these packages are available at
   http://www.appscio.com/repository/packages/release/SOURCES/.

This release replaces all pre-release packages.  Pre-release packages can be identified
by the following naming convention:
   <package_name>-<version>-snapshot.r<revision_number>.fc10.x86_64.rpm
To replace your pre-release packages:
1) cd /etc/yum.repos.d
2) sudo rm -f appscio.repo
3) wget http://www.appscio.com/repository/packages/release/fedora/10/RPMS/appscio.repo
4) yum clean all
5) yum update <mpf_package_name>

==============================================
REQUIREMENTS:
==============================================
See http://www.appscio.org/sites/default/files/Appscio-MPF-Installation-Guide.pdf for
information about system requirements

===============================================
INSTALLATION:
===============================================
See http://www.appscio.org/sites/default/files/Appscio-MPF-Installation-Guide.pdf for
detailed installation instructions.

===============================================
USAGE:
===============================================
See http://www.appscio.org/sites/default/files/Appscio-MPF-Development-Guide.pdf for
information on how to use MPF.

===============================================
ROAD MAP:
===============================================
The following enhancements are planned for the next release of MPF:

* Generalize approach to dealing with delayed-answer analytics.  Many analytics operate
  by taking in a stream of data then later issuing a result, whether by callback or by
  piggybacking on the return from the "process" call; the answer reflects data that is
  no longer available.

* Support parameter processing in component_init.


===============================================
VERSION HISTORY
===============================================
07-10-2009 - Initial version - Curt Patrick

