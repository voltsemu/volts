# stupid mac workaround hacky bullshit
main:
	cd Build && ninja && cd .. && \
	clang++ -ObjC++ -framework Cocoa Volts/Platform/Apple/Main.mm -I. Build/libVoltLib.a && \
	mv a.out Volt