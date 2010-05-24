Steps to generate html for the Appscio(TM) Component Writer's Guide
and the mpf-core html

copy all mpf-root files to MPF/trunk:
cd MPF/trunk
cp -R docs/reference/mpf-root/* .

execute autogen:
chmod ugo+x *.sh
./autogen.sh --enable-gtk-doc
cd mpf-core
./autogen.sh

make:
cd mpf-core
make
cd docs/reference/common
make

verify:
cd docs/reference/common/html
-- expect to see the generated html for Docbook SGML
