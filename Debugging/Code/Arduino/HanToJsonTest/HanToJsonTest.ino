/**
 * Simple sketch to simulate reading data from different meters.
 *
 * Created 24. October 2017 by Roar Fredriksen
 */

#include <ArduinoJson.h>
//#include "Kaifa.h"
//#include "Kamstrup.h"
//#include "Aidon.h"
#include "HanReader.h"
#include "HanToJson.h"

// The HAN Port reader
HanReader hanReader;

HardwareSerial* debugger = NULL;

byte meterType = 1; // Start with Kaifa
int  sampleIndex = 0;
int  expectIndex = 0;
byte kaifa_samples[] =
{
	// List #1
	0x7E, 0xA0, 0x27, 0x01,  0x02, 0x01, 0x10, 0x5A,  0x87, 0xE6, 0xE7, 0x00,  0x0F, 0x40, 0x00, 0x00,
	0x00, 0x09, 0x0C, 0x07,  0xE1, 0x09, 0x0E, 0x04,  0x13, 0x1F, 0x02, 0xFF,  0x80, 0x00, 0x00, 0x02,
	0x01, 0x06, 0x00, 0x00,  0x03, 0x98, 0xAB, 0xAD,  0x7E,

	// List#2
	0x7E, 0xA0, 0x79, 0x01,  0x02, 0x01, 0x10, 0x80,  0x93, 0xE6, 0xE7, 0x00,  0x0F, 0x40, 0x00, 0x00,
	0x00, 0x09, 0x0C, 0x07,  0xE1, 0x09, 0x0E, 0x04,  0x13, 0x1F, 0x0A, 0xFF,  0x80, 0x00, 0x00, 0x02,
	0x0D, 0x09, 0x07, 0x4B,  0x46, 0x4D, 0x5F, 0x30,  0x30, 0x31, 0x09, 0x10,  0x36, 0x39, 0x37, 0x30,
	0x36, 0x33, 0x31, 0x34,  0x30, 0x31, 0x37, 0x35,  0x33, 0x39, 0x38, 0x35,  0x09, 0x08, 0x4D, 0x41,
	0x33, 0x30, 0x34, 0x48,  0x33, 0x45, 0x06, 0x00,  0x00, 0x03, 0x96, 0x06,  0x00, 0x00, 0x00, 0x00,
	0x06, 0x00, 0x00, 0x00,  0x00, 0x06, 0x00, 0x00,  0x00, 0x20, 0x06, 0x00,  0x00, 0x05, 0x64, 0x06,
	0x00, 0x00, 0x0C, 0x92,  0x06, 0x00, 0x00, 0x0C,  0x49, 0x06, 0x00, 0x00,  0x09, 0x46, 0x06, 0x00,
	0x00, 0x00, 0x00, 0x06,  0x00, 0x00, 0x09, 0x4E,  0x1F, 0x85, 0x7E,

	// List#3
	0x7E, 0xA0, 0x9B, 0x01,  0x02, 0x01, 0x10, 0xEE,  0xAE, 0xE6, 0xE7, 0x00,  0x0F, 0x40, 0x00, 0x00,
	0x00, 0x09, 0x0C, 0x07,  0xE1, 0x09, 0x0E, 0x04,  0x14, 0x00, 0x0A, 0xFF,  0x80, 0x00, 0x00, 0x02,
	0x12, 0x09, 0x07, 0x4B,  0x46, 0x4D, 0x5F, 0x30,  0x30, 0x31, 0x09, 0x10,  0x36, 0x39, 0x37, 0x30,
	0x36, 0x33, 0x31, 0x34,  0x30, 0x31, 0x37, 0x35,  0x33, 0x39, 0x38, 0x35,  0x09, 0x08, 0x4D, 0x41,
	0x33, 0x30, 0x34, 0x48,  0x33, 0x45, 0x06, 0x00,  0x00, 0x03, 0xFE, 0x06,  0x00, 0x00, 0x00, 0x00,
	0x06, 0x00, 0x00, 0x00,  0x00, 0x06, 0x00, 0x00,  0x00, 0x40, 0x06, 0x00,  0x00, 0x07, 0x91, 0x06,
	0x00, 0x00, 0x0C, 0x9D,  0x06, 0x00, 0x00, 0x0D,  0x66, 0x06, 0x00, 0x00,  0x09, 0x41, 0x06, 0x00,
	0x00, 0x00, 0x00, 0x06,  0x00, 0x00, 0x09, 0x4C,  0x09, 0x0C, 0x07, 0xE1,  0x09, 0x0E, 0x04, 0x14,
	0x00, 0x0A, 0xFF, 0x80,  0x00, 0x00, 0x06, 0x00,  0x02, 0xBF, 0x69, 0x06,  0x00, 0x00, 0x00, 0x00,
	0x06, 0x00, 0x00, 0x00,  0xF7, 0x06, 0x00, 0x00,  0x3F, 0xFC, 0x71, 0x71,  0x7E,

	// List#2.1
	0x7e, 0xa0, 0x65, 0x01,  0x02, 0x01, 0x10, 0xf0,  0x50, 0xe6, 0xe7, 0x00,  0x0f, 0x40, 0x00, 0x00,
	0x00, 0x09, 0x0c, 0x07,  0xe3, 0x05, 0x08, 0x03,  0x16, 0x20, 0x28, 0xff,  0x80, 0x00, 0x00, 0x02,
	0x09, 0x09, 0x07, 0x4b,  0x46, 0x4d, 0x5f, 0x30,  0x30, 0x31, 0x09, 0x10,  0x36, 0x39, 0x37, 0x30,
	0x36, 0x33, 0x31, 0x34,  0x30, 0x31, 0x39, 0x39,  0x31, 0x36, 0x38, 0x34,  0x09, 0x08, 0x4d, 0x41,
	0x31, 0x30, 0x35, 0x48,  0x32, 0x45, 0x06, 0x00,  0x00, 0x03, 0xa5, 0x06,  0x00, 0x00, 0x00, 0x00,
	0x06, 0x00, 0x00, 0x00,  0x21, 0x06, 0x00, 0x00,  0x00, 0x00, 0x06, 0x00,  0x00, 0x10, 0x0e, 0x06,
	0x00, 0x00, 0x09, 0x0a,  0x98, 0x76, 0x7e,
};
String kaifa_expect[] = {
	String("{\"t\":1505417462,\"data\":{\"P\":920}}"),
	String("{\"t\":1505417470,\"data\":{\"lv\":\"KFM_001\",\"id\":\"6970631401753985\",\"type\":\"MA304H3E\",\"P\":918,\"Q\":0,\"I1\":1380,\"I2\":3218,\"I3\":3145,\"U1\":2374,\"U2\":0,\"U3\":2382}}"),
	String("{\"t\":1505419210,\"data\":{\"lv\":\"KFM_001\",\"id\":\"6970631401753985\",\"type\":\"MA304H3E\",\"P\":1022,\"Q\":0,\"I1\":1937,\"I2\":3229,\"I3\":3430,\"U1\":2369,\"U2\":0,\"U3\":2380,\"tPI\":180073,\"tPO\":0,\"tQI\":247,\"tQO\":16380}}"),
	String("{\"t\":1557354760,\"data\":{\"lv\":\"KFM_001\",\"id\":\"6970631401991684\",\"type\":\"MA105H2E\",\"P\":933,\"Q\":33,\"I1\":4110,\"U1\":2314}}")
};

byte aidon_samples[] =
{ // From Aidon-HAN-Interface-Description-v10A-ID-34331.pdf
	// List 2 sending (1-phase)
	0x7e, 0xa0, 0xd2, 0x41,  0x08, 0x83, 0x13, 0x82,  0xd6, 0xe6, 0xe7, 0x00,  0x0f, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x09,  0x02, 0x02, 0x09, 0x06,  0x01, 0x01, 0x00, 0x02,  0x81, 0xff, 0x0a, 0x0b,
	0x41, 0x49, 0x44, 0x4f,  0x4e, 0x5f, 0x56, 0x30,  0x30, 0x30, 0x31, 0x02,  0x02, 0x09, 0x06, 0x00,
	0x00, 0x60, 0x01, 0x00,  0xff, 0x0a, 0x10, 0x37,  0x33, 0x35, 0x39, 0x39,  0x39, 0x32, 0x38, 0x39,
	0x30, 0x39, 0x34, 0x31,  0x37, 0x34, 0x32, 0x02,  0x02, 0x09, 0x06, 0x00,  0x00, 0x60, 0x01, 0x07,
	0xff, 0x0a, 0x04, 0x36,  0x35, 0x31, 0x35, 0x02,  0x03, 0x09, 0x06, 0x01,  0x00, 0x01, 0x07, 0x00,
	0xff, 0x06, 0x00, 0x00,  0x05, 0x52, 0x02, 0x02,  0x0f, 0x00, 0x16, 0x1b,  0x02, 0x03, 0x09, 0x06,
	0x01, 0x00, 0x02, 0x07,  0x00, 0xff, 0x06, 0x00,  0x00, 0x00, 0x00, 0x02,  0x02, 0x0f, 0x00, 0x16,
	0x1b, 0x02, 0x03, 0x09,  0x06, 0x01, 0x00, 0x03,  0x07, 0x00, 0xff, 0x06,  0x00, 0x00, 0x03, 0xe4,
	0x02, 0x02, 0x0f, 0x00,  0x16, 0x1d, 0x02, 0x03,  0x09, 0x06, 0x01, 0x00,  0x04, 0x07, 0x00, 0xff,
	0x06, 0x00, 0x00, 0x00,  0x00, 0x02, 0x02, 0x0f,  0x00, 0x16, 0x1d, 0x02,  0x03, 0x09, 0x06, 0x01,
	0x00, 0x1f, 0x07, 0x00,  0xff, 0x10, 0x00, 0x5d,  0x02, 0x02, 0x0f, 0xff,  0x16, 0x21, 0x02, 0x03,
	0x09, 0x06, 0x01, 0x00,  0x20, 0x07, 0x00, 0xff,  0x12, 0x09, 0xc4, 0x02,  0x02, 0x0f, 0xff, 0x16,
	0x23, 0xe0, 0xc4, 0x7e
};
String aidon_expect[] = {
	String("{\"t\":0,\"data\":{\"lv\":\"AIDON_V0001\",\"id\":\"7359992890941742\",\"type\":\"6515\",\"P\":1362,\"Q\":0,\"I1\":9.3,\"U1\":250}}")
};

byte kamstrup_samples[] =
{ // [2017-10-20 04.43.32.368 - Received 229 (0xE5) bytes]
	// List #1
	0x7E, 0xA0, 0xE2, 0x2B,  0x21, 0x13, 0x23, 0x9A,  0xE6, 0xE7, 0x00, 0x0F,  0x00, 0x00, 0x00, 0x00,
	0x0C, 0x07, 0xE2, 0x03,  0x04, 0x07, 0x14, 0x3B,  0x32, 0xFF, 0x80, 0x00,  0x00, 0x02, 0x19, 0x0A,
	0x0E, 0x4B, 0x61, 0x6D,  0x73, 0x74, 0x72, 0x75,  0x70, 0x5F, 0x56, 0x30,  0x30, 0x30, 0x31, 0x09,
	0x06, 0x01, 0x01, 0x00,  0x00, 0x05, 0xFF, 0x0A,  0x10, 0x35, 0x37, 0x30,  0x36, 0x35, 0x36, 0x37,
	0x32, 0x37, 0x34, 0x33,  0x38, 0x39, 0x37, 0x30,  0x32, 0x09, 0x06, 0x01,  0x01, 0x60, 0x01, 0x01,
	0xFF, 0x0A, 0x12, 0x36,  0x38, 0x34, 0x31, 0x31,  0x32, 0x31, 0x42, 0x4E,  0x32, 0x34, 0x33, 0x31,
	0x30, 0x31, 0x30, 0x34,  0x30, 0x09, 0x06, 0x01,  0x01, 0x01, 0x07, 0x00,  0xFF, 0x06, 0x00, 0x00,
	0x11, 0x28, 0x09, 0x06,  0x01, 0x01, 0x02, 0x07,  0x00, 0xFF, 0x06, 0x00,  0x00, 0x00, 0x00, 0x09,
	0x06, 0x01, 0x01, 0x03,  0x07, 0x00, 0xFF, 0x06,  0x00, 0x00, 0x00, 0x84,  0x09, 0x06, 0x01, 0x01,
	0x04, 0x07, 0x00, 0xFF,  0x06, 0x00, 0x00, 0x00,  0x00, 0x09, 0x06, 0x01,  0x01, 0x1F, 0x07, 0x00,
	0xFF, 0x06, 0x00, 0x00,  0x05, 0x66, 0x09, 0x06,  0x01, 0x01, 0x33, 0x07,  0x00, 0xFF, 0x06, 0x00,
	0x00, 0x03, 0x0C, 0x09,  0x06, 0x01, 0x01, 0x47,  0x07, 0x00, 0xFF, 0x06,  0x00, 0x00, 0x05, 0x5A,
	0x09, 0x06, 0x01, 0x01,  0x20, 0x07, 0x00, 0xFF,  0x12, 0x00, 0xE0, 0x09,  0x06, 0x01, 0x01, 0x34,
	0x07, 0x00, 0xFF, 0x12,  0x00, 0xDF, 0x09, 0x06,  0x01, 0x01, 0x48, 0x07,  0x00, 0xFF, 0x12, 0x00,
	0xDF, 0xA1, 0xD8, 0x7E,

	// List #2
	0x7E, 0xA1, 0x2C, 0x2B,  0x21, 0x13, 0xFC, 0x04,  0xE6, 0xE7, 0x00, 0x0F,  0x00, 0x00, 0x00, 0x00,
	0x0C, 0x07, 0xE2, 0x03,  0x04, 0x07, 0x15, 0x00,  0x05, 0xFF, 0x80, 0x00,  0x00, 0x02, 0x23, 0x0A,
	0x0E, 0x4B, 0x61, 0x6D,  0x73, 0x74, 0x72, 0x75,  0x70, 0x5F, 0x56, 0x30,  0x30, 0x30, 0x31, 0x09,
	0x06, 0x01, 0x01, 0x00,  0x00, 0x05, 0xFF, 0x0A,  0x10, 0x35, 0x37, 0x30,  0x36, 0x35, 0x36, 0x37,
	0x32, 0x37, 0x34, 0x33,  0x38, 0x39, 0x37, 0x30,  0x32, 0x09, 0x06, 0x01,  0x01, 0x60, 0x01, 0x01,
	0xFF, 0x0A, 0x12, 0x36,  0x38, 0x34, 0x31, 0x31,  0x32, 0x31, 0x42, 0x4E,  0x32, 0x34, 0x33, 0x31,
	0x30, 0x31, 0x30, 0x34,  0x30, 0x09, 0x06, 0x01,  0x01, 0x01, 0x07, 0x00,  0xFF, 0x06, 0x00, 0x00,
	0x0E, 0x3B, 0x09, 0x06,  0x01, 0x01, 0x02, 0x07,  0x00, 0xFF, 0x06, 0x00,  0x00, 0x00, 0x00, 0x09,
	0x06, 0x01, 0x01, 0x03,  0x07, 0x00, 0xFF, 0x06,  0x00, 0x00, 0x00, 0x86,  0x09, 0x06, 0x01, 0x01,
	0x04, 0x07, 0x00, 0xFF,  0x06, 0x00, 0x00, 0x00,  0x00, 0x09, 0x06, 0x01,  0x01, 0x1F, 0x07, 0x00,
	0xFF, 0x06, 0x00, 0x00,  0x04, 0x21, 0x09, 0x06,  0x01, 0x01, 0x33, 0x07,  0x00, 0xFF, 0x06, 0x00,
	0x00, 0x03, 0x0C, 0x09,  0x06, 0x01, 0x01, 0x47,  0x07, 0x00, 0xFF, 0x06,  0x00, 0x00, 0x04, 0x1C,
	0x09, 0x06, 0x01, 0x01,  0x20, 0x07, 0x00, 0xFF,  0x12, 0x00, 0xE2, 0x09,  0x06, 0x01, 0x01, 0x34,
	0x07, 0x00, 0xFF, 0x12,  0x00, 0xE0, 0x09, 0x06,  0x01, 0x01, 0x48, 0x07,  0x00, 0xFF, 0x12, 0x00,
	0xDF, 0x09, 0x06, 0x00,  0x01, 0x01, 0x00, 0x00,  0xFF, 0x09, 0x0C, 0x07,  0xE2, 0x03, 0x04, 0x07,
	0x15, 0x00, 0x05, 0xFF,  0x80, 0x00, 0x00, 0x09,  0x06, 0x01, 0x01, 0x01,  0x08, 0x00, 0xFF, 0x06,
	0x00, 0x1A, 0x40, 0x49,  0x09, 0x06, 0x01, 0x01,  0x02, 0x08, 0x00, 0xFF,  0x06, 0x00, 0x00, 0x00,
	0x00, 0x09, 0x06, 0x01,  0x01, 0x03, 0x08, 0x00,  0xFF, 0x06, 0x00, 0x00,  0x05, 0x64, 0x09, 0x06,
	0x01, 0x01, 0x04, 0x08,  0x00, 0xFF, 0x06, 0x00,  0x02, 0x7B, 0x21, 0x20,  0x92, 0x7E
};
String kamstrup_expect[] = {
	String("{\"t\":1520197190,\"data\":{\"lv\":\"Kamstrup_V0001\",\"id\":\"5706567274389702\",\"type\":\"6841121BN243101040\",\"P\":4392,\"Q\":132,\"I1\":1382,\"I2\":780,\"I3\":1370,\"U1\":224,\"U2\":223,\"U3\":223}}"),
	String("{\"t\":1520197205,\"data\":{\"lv\":\"Kamstrup_V0001\",\"id\":\"5706567274389702\",\"type\":\"6841121BN243101040\",\"P\":3643,\"Q\":134,\"I1\":1057,\"I2\":780,\"I3\":1052,\"U1\":226,\"U2\":224,\"U3\":223,\"tPI\":1720393,\"tPO\":0,\"tQI\":1380,\"tQO\":162593}}")
};


void assert_equal(DynamicJsonDocument& doc, const String& expected)
{
	String jsonActual;
	serializeJson(doc, jsonActual);
	if (jsonActual != expected)
	{
		debugger->printf("Test assert failed:\n  %s\n  !=\n  %s\n", jsonActual.c_str(), expected.c_str());
		while (true) {}
	}
}


void setup() {
	Serial1.begin(115200);
	//while (!Serial) {}
	Serial1.println("Serial1 debugging port initialized");

	// initialize the HanReader
	// (passing no han port, as we are feeding data manually, but provide Serial for debugging)
	hanReader.setup(NULL, &Serial1);
	hanReader.compensateFor09HeaderBug = true; // Starting with Kaifa

	debugger = &Serial1;
}

void loopKaifa()
{
	if (sampleIndex >= sizeof(kaifa_samples))
	{
		meterType++;
		sampleIndex = 0;
		expectIndex = 0;
		hanReader.setup(NULL, debugger);
		hanReader.compensateFor09HeaderBug = false;
		Serial1.println("Done with Kaifa");
	}

	// Read one byte from the "port", and see if we got a full package
	if (hanReader.read(kaifa_samples[sampleIndex++]))
	{
		DynamicJsonDocument doc(500);

		doc["t"] = hanReader.getPackageTime();

		JsonObject data = doc.createNestedObject("data");

		hanToJson(data, meterType, hanReader);

		debugger->println("Kaifa JsonData: ");
		serializeJsonPretty(doc, Serial1);
		debugger->println();

		if (expectIndex < sizeof(kaifa_expect) / sizeof(kaifa_expect[0]))
		{
			assert_equal(doc, kaifa_expect[expectIndex++]);
		} else {
			debugger->println("Not enough expected results spesified");
			while (true) {}
		}
	}
}

void loopAidon()
{
	if (sampleIndex >= sizeof(aidon_samples))
	{
		meterType++;
		sampleIndex = 0;
		expectIndex = 0;
		hanReader.setup(NULL, &Serial1);
		debugger->println("Done with Aidon");
	}

	// Read one byte from the "port", and see if we got a full package
	if (hanReader.read(aidon_samples[sampleIndex++]))
	{
		DynamicJsonDocument doc(500);

		doc["t"] = hanReader.getPackageTime();

		JsonObject data = doc.createNestedObject("data");

		hanToJson(data, meterType, hanReader);

		debugger->println("Aidon JsonData: ");
		serializeJsonPretty(doc, Serial1);
		debugger->println();

		if (expectIndex < sizeof(aidon_expect) / sizeof(aidon_expect[0]))
		{
			assert_equal(doc, aidon_expect[expectIndex++]);
		} else {
			debugger->println("Not enough expected results spesified");
			while (true) {}
		}
	}
}

void loopKamstrup()
{
	if (sampleIndex >= sizeof(kamstrup_samples))
	{
		meterType++;
		sampleIndex = 0;
		hanReader.setup(NULL, &Serial1);
		debugger->println("Done with Kamstrup");
	}

	// Read one byte from the "port", and see if we got a full package
	if (hanReader.read(kamstrup_samples[sampleIndex++]))
	{
		DynamicJsonDocument doc(500);

		doc["t"] = hanReader.getPackageTime();

		JsonObject data = doc.createNestedObject("data");

		hanToJson(data, meterType, hanReader);

		debugger->println("Kamstrup JsonData: ");
		serializeJsonPretty(doc, Serial1);
		debugger->println();

		if (expectIndex < sizeof(kamstrup_expect) / sizeof(kamstrup_expect[0]))
		{
			assert_equal(doc, kamstrup_expect[expectIndex++]);
		} else {
			debugger->println("Not enough expected results spesified");
			while (true) {}
		}
	}
}

void loop() {
	switch (meterType)
	{
		case 1: // Kaifa
			return loopKaifa();
		case 2: // Aidon
			return loopAidon();
		case 3: // Kamstrup
			return loopKamstrup();
		default:
			debugger->println("Done");
			while (true) ;
			break;
	}
}
