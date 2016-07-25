# $Id: CMakeLists.txt 1355 2016-02-17 14:43:14Z klugeflo $

set(CFG_FILES
	bdlp.mkg
	short.mkg
	default.mkg
	long.mkg

	feas-m-0.5.mkg
	feas-m-0.6.mkg
	feas-m-0.7.mkg
	feas-m-0.8.mkg
	feas-m-0.9.mkg
	feas-m.mkg

	lp-feas-m-0.1.mkg
	lp-feas-m-0.2.mkg
	lp-feas-m-0.3.mkg
	lp-feas-m-0.4.mkg
	lp-feas-m-0.5.mkg
	lp-feas-m-0.6.mkg
	lp-feas-m-0.7.mkg
	lp-feas-m-0.8.mkg
	lp-feas-m-0.9.mkg

	default.econf
	no-ec.econf
	)

install(FILES ${CFG_FILES} DESTINATION ${CFG_INSTALL_DIR})
