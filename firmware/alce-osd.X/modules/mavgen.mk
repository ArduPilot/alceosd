# mavlink headers generation

ROOT=.
RM=rm -rf 

MAVLINK_DIR := $(ROOT)/modules/mavlink/
MESSAGE_DEFINITIONS := $(ROOT)/modules/mavlink/message_definitions/v1.0
MAVLINK_HEADERS := $(ROOT)/include/mavlink/v1.0/ardupilotmega/mavlink.h $(wildcard $(ROOT)/include/mavlink/v1.0/,*.h) $(wildcard $(ROOT)/include/mavlink/v1.0/ardupilotmega,*.h)
MAVLINK_OUTPUT_DIR := $(ROOT)/include/mavlink/v1.0

.clean-pre:
	${RM} $(MAVLINK_OUTPUT_DIR)

.build-pre: $(MAVLINK_HEADERS)

$(MAVLINK_HEADERS): $(MESSAGE_DEFINITIONS)/ardupilotmega.xml $(MESSAGE_DEFINITIONS)/common.xml
	echo "Generating MAVLink headers..."
	echo "Generating C code using mavgen.py located at" $(ROOT)/modules/mavlink/
	-PYTHONPATH=$(MAVLINK_DIR) python $(MAVLINK_DIR)/pymavlink/tools/mavgen.py --lang=C --wire-protocol=1.0 --output=$(MAVLINK_OUTPUT_DIR) $(MAVLINK_DIR)/message_definitions/v1.0/ardupilotmega.xml

