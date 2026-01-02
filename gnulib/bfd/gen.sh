#!/bin/bash -e

URL="https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.xz"
ZFN=$(basename $URL)
DIRN=${ZFN%.tar*}

if [ ! -f $ZFN ] ; then
	curl -o $ZFN -L "$URL"
fi
if [ ! -d $DIRN ] ; then
	tar -xkf $ZFN
	pushd $DIRN >&/dev/null
	patch -p1 < ~/work/linux/patches/binutils/binutils.patch
	patch -p1 < ../msvc/1.patch
	popd >&/dev/null
fi

gen_nn(){
	sed -e "s/NN/$1/g" <$3 >$2
}
gen_xx(){
	sed -e "s/XX/$1/g" <$3 >$2
}

gen_nnsrc(){
	pushd $DIRN/bfd >&/dev/null
	gen_nn 32 elf32-target.h elfxx-target.h
	gen_nn 64 elf64-target.h elfxx-target.h
	gen_nn 32 elf32-aarch64.c elfnn-aarch64.c
	gen_nn 64 elf64-aarch64.c elfnn-aarch64.c
	gen_nn 32 elf32-ia64.c elfnn-ia64.c
	gen_nn 64 elf64-ia64.c elfnn-ia64.c
	gen_nn 32 elf32-kvx.c elfnn-kvx.c
	gen_nn 64 elf64-kvx.c elfnn-kvx.c
	gen_nn 32 elf32-loongarch.c elfnn-loongarch.c
	gen_nn 64 elf64-loongarch.c elfnn-loongarch.c
	gen_nn 32 elf32-riscv.c elfnn-riscv.c
	gen_nn 64 elf64-riscv.c elfnn-riscv.c

	gen_xx pe peigen.c peXXigen.c
	gen_xx pep pepigen.c peXXigen.c
	gen_xx pex64 pex64igen.c peXXigen.c
	gen_xx peAArch64 pe-aarch64igen.c peXXigen.c
	gen_xx peLoongArch64 pe-loongarch64igen.c peXXigen.c
	gen_xx peRiscV64 pe-riscv64igen.c peXXigen.c

	cat >bfdver.h<<'EOF'
#define BFD_VERSION_DATE 20240129
#define BFD_VERSION 242000000
#define BFD_VERSION_STRING  "binutils 2.42 (msvc)"
#define REPORT_BUGS_TO "<_>"
EOF

	sed -f targmatch.sed < config.bfd > targmatch.h
}

if [ ! -f $DIRN/bfd/targmatch.h ] ; then
	gen_nnsrc
fi
echo OK!
