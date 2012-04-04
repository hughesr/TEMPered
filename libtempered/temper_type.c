#include <stdlib.h>
#include <stdbool.h>

#include "temper_type.h"

#include "tempered.h"
#include "type_hid/type-info.h"
#include "type_hid/common.h"
#include "type_hid/fm75.h"
#include "type_hid/sht1x.h"
#include "type_hid/ntc.h"

// This is an array of known TEMPer types.
struct temper_type known_temper_types[]={
	{
		.name="TEMPer2HumiV1.x",
		.vendor_id=0x0c45,
		.product_id=0x7402,
		.interface_number=0
	},
	{
		.name="TEMPer2HumiV1.x",
		.vendor_id=0x0c45,
		.product_id=0x7402,
		.interface_number=1,
		.open = temper_type_hid_open,
		.close = temper_type_hid_close,
		.get_subtype_id = NULL,
		.subtypes = (struct temper_subtype*[]){
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.name = "TEMPer2HumiV1.x",
					.open = temper_type_hid_subtype_open,
					.read_sensors = temper_type_hid_read_sensors,
					.get_temperature = temper_type_hid_get_temperature,
					.get_humidity = temper_type_hid_get_humidity
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query.length = 8,
						.query.data = (unsigned char[]){ 1, 0x80, 0x33, 1, 0, 0, 0, 0 },
						.read_sensors = temper_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_sht1x,
								.get_humidity = tempered_type_hid_get_humidity_sht1x,
								.temperature_high_byte_offset = 2,
								.temperature_low_byte_offset = 3,
								.humidity_high_byte_offset = 4,
								.humidity_low_byte_offset = 5
							}
						}
					}
				}
			},
			NULL // List terminator for subtypes
		}
	},
	{
		.name="TEMPerV1.2 or TEMPer2.2",
		.vendor_id=0x0c45,
		.product_id=0x7401,
		.interface_number=0
	},
	{
		.name="TEMPerV1.2 or TEMPer2.2",
		.vendor_id=0x0c45,
		.product_id=0x7401,
		.interface_number=1,
		.open = temper_type_hid_open,
		.close = temper_type_hid_close,
		.get_subtype_id = temper_type_hid_get_subtype_id,
		// TODO: find the id_query and offset for this type
		.get_subtype_data = NULL,
		.subtypes = (struct temper_subtype*[]){
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0, // TODO: find the ID
					.name = "TEMPerV1.2",
					.open = temper_type_hid_subtype_open,
					.read_sensors = temper_type_hid_read_sensors,
					.get_temperature = temper_type_hid_get_temperature,
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query.length = 8,
						.query.data = (unsigned char[]){ 1, 0x80, 0x33, 1, 0, 0, 0, 0 },
						.read_sensors = temper_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 2,
								.temperature_low_byte_offset = 3
							}
						}
					}
				}
			},
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0, // TODO: find the ID
					.name = "TEMPer2.2",
					.open = temper_type_hid_subtype_open,
					.read_sensors = temper_type_hid_read_sensors,
					.get_sensor_count = temper_type_hid_get_sensor_count,
					.get_temperature = temper_type_hid_get_temperature,
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query.length = 8,
						.query.data = (unsigned char[]){ 1, 0x80, 0x33, 1, 0, 0, 0, 0 },
						.read_sensors = temper_type_hid_read_sensor_group,
						.sensor_count = 2,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 2,
								.temperature_low_byte_offset = 3
							},
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 4,
								.temperature_low_byte_offset = 5
							}
						}
					}
				}
			},
			NULL // List terminator for subtypes
		}
	},
	{
		.name="HidTEMPer1, 2, NTC or HUM",
		.vendor_id=0x1130,
		.product_id=0x660c,
		.interface_number=0
	},
	{
		.name="HidTEMPer1, 2, NTC or HUM",
		.vendor_id=0x1130,
		.product_id=0x660c,
		.interface_number=1,
		.open = temper_type_hid_open,
		.close = temper_type_hid_close,
		.get_subtype_id = temper_type_hid_get_subtype_id,
		.get_subtype_data = &(struct tempered_type_hid_subtype_data){
			.id_offset = 2,
			.query.length = 8,
			.query.data = (unsigned char[]){ 0x52, 0, 0, 0, 0, 0, 0, 0 }
			// calibration is in offsets 2,3 for internal and 4,5 for external
			// sensor, at least for some types - but I don't know how to use it
		},
		.subtypes = (struct temper_subtype*[]){
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0x58,
					// TODO: use .open to initialize the high-resolution data
					// (by sending 0x43 to the device)
					.open = temper_type_hid_subtype_open,
					.name = "HidTEMPer1 (experimental)",
					.read_sensors = temper_type_hid_read_sensors,
					.get_temperature = temper_type_hid_get_temperature
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query.length = 8,
						.query.data = (unsigned char[]){ 0x54, 0, 0, 0, 0, 0, 0, 0 },
						.read_sensors = temper_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1
							}
						}
					}
				}
			},
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0x59,
					.name = "HidTEMPer2 (experimental)",
					.open = temper_type_hid_subtype_open,
					.read_sensors = temper_type_hid_read_sensors,
					.get_sensor_count = temper_type_hid_get_sensor_count,
					.get_temperature = temper_type_hid_get_temperature
				},
				.sensor_group_count = 2,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{ // Internal sensor
						.query.length = 8,
						.query.data = (unsigned char[]){ 0x54, 0, 0, 0, 0, 0, 0, 0 },
						.read_sensors = temper_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1
							}
						}
					},
					{ // External sensor
						.query.length = 8,
						.query.data = (unsigned char[]){ 0x53, 0, 0, 0, 0, 0, 0, 0 },
						.read_sensors = temper_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1
							}
						}
					}
				}
			},
			/*
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0x00, // TODO: find the ID
					.name = "HidTEMPerHUM (experimental)",
					.open = temper_type_hid_subtype_open,
					.read_sensors = temper_type_hid_read_sensors,
					.get_temperature = temper_type_hid_get_temperature,
					.get_humidity = temper_type_hid_get_humidity
				},
				.sensor_group_count = 1,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{
						.query.length = 8,
						.query.data = (unsigned char[]){ 0x48, 0, 0, 0, 0, 0, 0, 0 },
						.read_sensors = temper_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_sht1x,
								.get_humidity = tempered_type_hid_get_humidity_sht1x,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1
								.humidity_high_byte_offset = 2,
								.humidity_low_byte_offset = 3
							}
						}
					}
				}
			},
			*/
			(struct temper_subtype*)&(struct temper_subtype_hid){
				.base = {
					.id = 0x5b,
					.name = "HidTEMPerNTC (experimental)",
					.open = temper_type_hid_subtype_open,
					.read_sensors = temper_type_hid_read_sensors,
					.get_sensor_count = temper_type_hid_get_sensor_count,
					.get_temperature = temper_type_hid_get_temperature
				},
				.sensor_group_count = 2,
				.sensor_groups = (struct tempered_type_hid_sensor_group[]){
					{ // Internal sensor
						.query.length = 8,
						.query.data = (unsigned char[]){ 0x54, 0, 0, 0, 0, 0, 0, 0 },
						.read_sensors = temper_type_hid_read_sensor_group,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_fm75,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1
							}
						}
					},
					{ // External sensor
						.query.length = 8,
						.query.data = (unsigned char[]){ 0x41, 0, 0, 0, 0, 0, 0, 0 },
						.read_sensors = tempered_type_hid_read_sensor_group_ntc,
						.sensor_count = 1,
						.sensors = (struct tempered_type_hid_sensor[]){
							{
								.get_temperature = tempered_type_hid_get_temperature_ntc,
								.temperature_high_byte_offset = 0,
								.temperature_low_byte_offset = 1,
							}
						}
					}
				}
			},
			NULL // List terminator for subtypes
		}
	},
	{ .name=NULL } // List terminator for temper types
};

// Get the temper_type that matches the given USB device information
struct temper_type* temper_type_find(
	unsigned short vendor_id, unsigned short product_id, int interface_number
) {
	struct temper_type *type;
	for ( type = known_temper_types; type->name != NULL; type++ )
	{
		if (
			type->vendor_id == vendor_id &&
			type->product_id == product_id &&
			type->interface_number == interface_number
		) {
			return type;
		}
	}
	return NULL;
}

// Find the subtype of the given temper_type that has the given subtype ID.
struct temper_subtype* temper_type_find_subtype(
	struct temper_type const * type, unsigned char subtype_id
) {
	int i = 0;
	while ( type->subtypes[i] != NULL && type->subtypes[i]->id != subtype_id )
	{
		i++;
	}
	return type->subtypes[i];
}

/** Initialize the TEMPer types. */
bool temper_type_init( char **error )
{
	return temper_type_hid_init( error );
}

/** Finalize the TEMPer types. */
bool temper_type_exit( char **error )
{
	return temper_type_hid_exit( error );
}

/** Enumerate the known TEMPer devices. */
struct tempered_device_list* temper_type_enumerate( char **error )
{
	return temper_type_hid_enumerate( error );
}
