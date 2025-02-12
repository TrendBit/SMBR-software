#pragma once

#define CALL_AT_STARTUP(NAME, BLOCK) \
	namespace { \
		namespace { \
			static const class Class##NAME { \
			public: \
				Class##NAME() BLOCK \
			} NAME; \
		} \
	}

