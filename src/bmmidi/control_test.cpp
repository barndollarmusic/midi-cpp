#include "bmmidi/control.hpp"

#include <limits>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bmmidi/data_value.hpp"

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

constexpr int kIntMin = std::numeric_limits<int>::min();
constexpr int kIntMax = std::numeric_limits<int>::max();

TEST(Control, ShouldProvideAliasedValues) {
  // LSB aliases:
  EXPECT_THAT(bmmidi::Control::kLsb000, Eq(bmmidi::Control::k032));
  EXPECT_THAT(bmmidi::Control::kLsb001, Eq(bmmidi::Control::k033));
  EXPECT_THAT(bmmidi::Control::kLsb002, Eq(bmmidi::Control::k034));
  EXPECT_THAT(bmmidi::Control::kLsb003, Eq(bmmidi::Control::k035));
  EXPECT_THAT(bmmidi::Control::kLsb004, Eq(bmmidi::Control::k036));
  EXPECT_THAT(bmmidi::Control::kLsb005, Eq(bmmidi::Control::k037));
  EXPECT_THAT(bmmidi::Control::kLsb006, Eq(bmmidi::Control::k038));
  EXPECT_THAT(bmmidi::Control::kLsb007, Eq(bmmidi::Control::k039));
  EXPECT_THAT(bmmidi::Control::kLsb008, Eq(bmmidi::Control::k040));
  EXPECT_THAT(bmmidi::Control::kLsb009, Eq(bmmidi::Control::k041));
  EXPECT_THAT(bmmidi::Control::kLsb010, Eq(bmmidi::Control::k042));
  EXPECT_THAT(bmmidi::Control::kLsb011, Eq(bmmidi::Control::k043));
  EXPECT_THAT(bmmidi::Control::kLsb012, Eq(bmmidi::Control::k044));
  EXPECT_THAT(bmmidi::Control::kLsb013, Eq(bmmidi::Control::k045));
  EXPECT_THAT(bmmidi::Control::kLsb014, Eq(bmmidi::Control::k046));
  EXPECT_THAT(bmmidi::Control::kLsb015, Eq(bmmidi::Control::k047));

  EXPECT_THAT(bmmidi::Control::kLsb016, Eq(bmmidi::Control::k048));
  EXPECT_THAT(bmmidi::Control::kLsb017, Eq(bmmidi::Control::k049));
  EXPECT_THAT(bmmidi::Control::kLsb018, Eq(bmmidi::Control::k050));
  EXPECT_THAT(bmmidi::Control::kLsb019, Eq(bmmidi::Control::k051));
  EXPECT_THAT(bmmidi::Control::kLsb020, Eq(bmmidi::Control::k052));
  EXPECT_THAT(bmmidi::Control::kLsb021, Eq(bmmidi::Control::k053));
  EXPECT_THAT(bmmidi::Control::kLsb022, Eq(bmmidi::Control::k054));
  EXPECT_THAT(bmmidi::Control::kLsb023, Eq(bmmidi::Control::k055));
  EXPECT_THAT(bmmidi::Control::kLsb024, Eq(bmmidi::Control::k056));
  EXPECT_THAT(bmmidi::Control::kLsb025, Eq(bmmidi::Control::k057));
  EXPECT_THAT(bmmidi::Control::kLsb026, Eq(bmmidi::Control::k058));
  EXPECT_THAT(bmmidi::Control::kLsb027, Eq(bmmidi::Control::k059));
  EXPECT_THAT(bmmidi::Control::kLsb028, Eq(bmmidi::Control::k060));
  EXPECT_THAT(bmmidi::Control::kLsb029, Eq(bmmidi::Control::k061));
  EXPECT_THAT(bmmidi::Control::kLsb030, Eq(bmmidi::Control::k062));
  EXPECT_THAT(bmmidi::Control::kLsb031, Eq(bmmidi::Control::k063));

  // Named aliases:
  EXPECT_THAT(bmmidi::Control::kBankSelect, Eq(bmmidi::Control::k000));
  EXPECT_THAT(bmmidi::Control::kModWheel, Eq(bmmidi::Control::k001));
  EXPECT_THAT(bmmidi::Control::kBreath, Eq(bmmidi::Control::k002));
  EXPECT_THAT(bmmidi::Control::kFoot, Eq(bmmidi::Control::k004));
  EXPECT_THAT(bmmidi::Control::kPortamentoTime, Eq(bmmidi::Control::k005));
  EXPECT_THAT(bmmidi::Control::kDataEntry, Eq(bmmidi::Control::k006));
  EXPECT_THAT(bmmidi::Control::kChannelVolume, Eq(bmmidi::Control::k007));
  EXPECT_THAT(bmmidi::Control::kBalance, Eq(bmmidi::Control::k008));
  EXPECT_THAT(bmmidi::Control::kPan, Eq(bmmidi::Control::k010));
  EXPECT_THAT(bmmidi::Control::kExpression, Eq(bmmidi::Control::k011));
  EXPECT_THAT(bmmidi::Control::kEffect1, Eq(bmmidi::Control::k012));
  EXPECT_THAT(bmmidi::Control::kEffect2, Eq(bmmidi::Control::k013));
  EXPECT_THAT(bmmidi::Control::kGeneralPurpose1, Eq(bmmidi::Control::k016));
  EXPECT_THAT(bmmidi::Control::kGeneralPurpose2, Eq(bmmidi::Control::k017));
  EXPECT_THAT(bmmidi::Control::kGeneralPurpose3, Eq(bmmidi::Control::k018));
  EXPECT_THAT(bmmidi::Control::kGeneralPurpose4, Eq(bmmidi::Control::k019));

  EXPECT_THAT(bmmidi::Control::kLsbBankSelect, Eq(bmmidi::Control::k032));
  EXPECT_THAT(bmmidi::Control::kLsbModWheel, Eq(bmmidi::Control::k033));
  EXPECT_THAT(bmmidi::Control::kLsbBreath, Eq(bmmidi::Control::k034));
  EXPECT_THAT(bmmidi::Control::kLsbFoot, Eq(bmmidi::Control::k036));
  EXPECT_THAT(bmmidi::Control::kLsbPortamentoTime, Eq(bmmidi::Control::k037));
  EXPECT_THAT(bmmidi::Control::kLsbDataEntry, Eq(bmmidi::Control::k038));
  EXPECT_THAT(bmmidi::Control::kLsbChannelVolume, Eq(bmmidi::Control::k039));
  EXPECT_THAT(bmmidi::Control::kLsbBalance, Eq(bmmidi::Control::k040));
  EXPECT_THAT(bmmidi::Control::kLsbPan, Eq(bmmidi::Control::k042));
  EXPECT_THAT(bmmidi::Control::kLsbExpression, Eq(bmmidi::Control::k043));
  EXPECT_THAT(bmmidi::Control::kLsbEffect1, Eq(bmmidi::Control::k044));
  EXPECT_THAT(bmmidi::Control::kLsbEffect2, Eq(bmmidi::Control::k045));
  EXPECT_THAT(bmmidi::Control::kLsbGeneralPurpose1, Eq(bmmidi::Control::k048));
  EXPECT_THAT(bmmidi::Control::kLsbGeneralPurpose2, Eq(bmmidi::Control::k049));
  EXPECT_THAT(bmmidi::Control::kLsbGeneralPurpose3, Eq(bmmidi::Control::k050));
  EXPECT_THAT(bmmidi::Control::kLsbGeneralPurpose4, Eq(bmmidi::Control::k051));

  EXPECT_THAT(bmmidi::Control::kSustainPedal, Eq(bmmidi::Control::k064));
  EXPECT_THAT(bmmidi::Control::kPortamentoSwitch, Eq(bmmidi::Control::k065));
  EXPECT_THAT(bmmidi::Control::kSostenutoPedal, Eq(bmmidi::Control::k066));
  EXPECT_THAT(bmmidi::Control::kSoftPedal, Eq(bmmidi::Control::k067));
  EXPECT_THAT(bmmidi::Control::kLegatoSwitch, Eq(bmmidi::Control::k068));
  EXPECT_THAT(bmmidi::Control::kHold2, Eq(bmmidi::Control::k069));

  EXPECT_THAT(bmmidi::Control::kSoundVariation, Eq(bmmidi::Control::k070));
  EXPECT_THAT(bmmidi::Control::kSoundController01, Eq(bmmidi::Control::k070));

  EXPECT_THAT(bmmidi::Control::kTimbreOrResonance, Eq(bmmidi::Control::k071));
  EXPECT_THAT(bmmidi::Control::kSoundController02, Eq(bmmidi::Control::k071));

  EXPECT_THAT(bmmidi::Control::kReleaseTime, Eq(bmmidi::Control::k072));
  EXPECT_THAT(bmmidi::Control::kSoundController03, Eq(bmmidi::Control::k072));

  EXPECT_THAT(bmmidi::Control::kAttackTime, Eq(bmmidi::Control::k073));
  EXPECT_THAT(bmmidi::Control::kSoundController04, Eq(bmmidi::Control::k073));

  EXPECT_THAT(bmmidi::Control::kBrightness, Eq(bmmidi::Control::k074));
  EXPECT_THAT(bmmidi::Control::kSoundController05, Eq(bmmidi::Control::k074));

  EXPECT_THAT(bmmidi::Control::kDecayTime, Eq(bmmidi::Control::k075));
  EXPECT_THAT(bmmidi::Control::kSoundController06, Eq(bmmidi::Control::k075));

  EXPECT_THAT(bmmidi::Control::kVibratoRate, Eq(bmmidi::Control::k076));
  EXPECT_THAT(bmmidi::Control::kSoundController07, Eq(bmmidi::Control::k076));

  EXPECT_THAT(bmmidi::Control::kVibratoDepth, Eq(bmmidi::Control::k077));
  EXPECT_THAT(bmmidi::Control::kSoundController08, Eq(bmmidi::Control::k077));

  EXPECT_THAT(bmmidi::Control::kVibratoDelay, Eq(bmmidi::Control::k078));
  EXPECT_THAT(bmmidi::Control::kSoundController09, Eq(bmmidi::Control::k078));

  EXPECT_THAT(bmmidi::Control::kSoundController10, Eq(bmmidi::Control::k079));

  EXPECT_THAT(bmmidi::Control::kGeneralPurpose5, Eq(bmmidi::Control::k080));
  EXPECT_THAT(bmmidi::Control::kGeneralPurpose6, Eq(bmmidi::Control::k081));
  EXPECT_THAT(bmmidi::Control::kGeneralPurpose7, Eq(bmmidi::Control::k082));
  EXPECT_THAT(bmmidi::Control::kGeneralPurpose8, Eq(bmmidi::Control::k083));
  EXPECT_THAT(bmmidi::Control::kPortamentoControl, Eq(bmmidi::Control::k084));

  EXPECT_THAT(bmmidi::Control::kLsbVelocity, Eq(bmmidi::Control::k088));

  EXPECT_THAT(bmmidi::Control::kReverbLevel, Eq(bmmidi::Control::k091));
  EXPECT_THAT(bmmidi::Control::kEffect1Depth, Eq(bmmidi::Control::k091));

  EXPECT_THAT(bmmidi::Control::kTremolo, Eq(bmmidi::Control::k092));
  EXPECT_THAT(bmmidi::Control::kEffect2Depth, Eq(bmmidi::Control::k092));

  EXPECT_THAT(bmmidi::Control::kChorusLevel, Eq(bmmidi::Control::k093));
  EXPECT_THAT(bmmidi::Control::kEffect3Depth, Eq(bmmidi::Control::k093));

  EXPECT_THAT(bmmidi::Control::kDetune, Eq(bmmidi::Control::k094));
  EXPECT_THAT(bmmidi::Control::kEffect4Depth, Eq(bmmidi::Control::k094));

  EXPECT_THAT(bmmidi::Control::kPhaser, Eq(bmmidi::Control::k095));
  EXPECT_THAT(bmmidi::Control::kEffect5Depth, Eq(bmmidi::Control::k095));

  EXPECT_THAT(bmmidi::Control::kDataIncrement, Eq(bmmidi::Control::k096));
  EXPECT_THAT(bmmidi::Control::kDataDecrement, Eq(bmmidi::Control::k097));
  EXPECT_THAT(bmmidi::Control::kLsbNRPN, Eq(bmmidi::Control::k098));
  EXPECT_THAT(bmmidi::Control::kNRPN, Eq(bmmidi::Control::k099));
  EXPECT_THAT(bmmidi::Control::kLsbRPN, Eq(bmmidi::Control::k100));
  EXPECT_THAT(bmmidi::Control::kRPN, Eq(bmmidi::Control::k101));

  EXPECT_THAT(bmmidi::Control::kAllSoundOff, Eq(bmmidi::Control::k120));
  EXPECT_THAT(bmmidi::Control::kResetAllControllers, Eq(bmmidi::Control::k121));
  EXPECT_THAT(bmmidi::Control::kLocalControlSwitch, Eq(bmmidi::Control::k122));
  EXPECT_THAT(bmmidi::Control::kAllNotesOff, Eq(bmmidi::Control::k123));
  EXPECT_THAT(bmmidi::Control::kOmniModeOff, Eq(bmmidi::Control::k124));
  EXPECT_THAT(bmmidi::Control::kOmniModeOn, Eq(bmmidi::Control::k125));
  EXPECT_THAT(bmmidi::Control::kMonoMode, Eq(bmmidi::Control::k126));
  EXPECT_THAT(bmmidi::Control::kPolyMode, Eq(bmmidi::Control::k127));
}

TEST(Control, ShouldValidateNumberRange) {
  EXPECT_THAT(bmmidi::controlNumberIsInRange(kIntMin), IsFalse());
  EXPECT_THAT(bmmidi::controlNumberIsInRange(-128), IsFalse());
  EXPECT_THAT(bmmidi::controlNumberIsInRange(-127), IsFalse());
  EXPECT_THAT(bmmidi::controlNumberIsInRange(-1), IsFalse());

  EXPECT_THAT(bmmidi::controlNumberIsInRange(0), IsTrue());
  EXPECT_THAT(bmmidi::controlNumberIsInRange(1), IsTrue());
  EXPECT_THAT(bmmidi::controlNumberIsInRange(126), IsTrue());
  EXPECT_THAT(bmmidi::controlNumberIsInRange(127), IsTrue());

  EXPECT_THAT(bmmidi::controlNumberIsInRange(128), IsFalse());
  EXPECT_THAT(bmmidi::controlNumberIsInRange(kIntMax), IsFalse());
}

TEST(Control, ShouldConvertToAndFromNumber) {
  EXPECT_THAT(bmmidi::controlFromNumber(0), Eq(bmmidi::Control::k000));
  EXPECT_THAT(bmmidi::controlToNumber(bmmidi::Control::k000), Eq(0));
  EXPECT_THAT(bmmidi::controlToNumber(bmmidi::Control::kBankSelect), Eq(0));

  EXPECT_THAT(bmmidi::controlFromNumber(1), Eq(bmmidi::Control::k001));
  EXPECT_THAT(bmmidi::controlToNumber(bmmidi::Control::k001), Eq(1));
  EXPECT_THAT(bmmidi::controlToNumber(bmmidi::Control::kModWheel), Eq(1));

  EXPECT_THAT(bmmidi::controlFromNumber(11), Eq(bmmidi::Control::k011));
  EXPECT_THAT(bmmidi::controlToNumber(bmmidi::Control::k011), Eq(11));
  EXPECT_THAT(bmmidi::controlToNumber(bmmidi::Control::kExpression), Eq(11));

  EXPECT_THAT(bmmidi::controlFromNumber(126), Eq(bmmidi::Control::k126));
  EXPECT_THAT(bmmidi::controlToNumber(bmmidi::Control::k126), Eq(126));
  EXPECT_THAT(bmmidi::controlToNumber(bmmidi::Control::kMonoMode), Eq(126));

  EXPECT_THAT(bmmidi::controlFromNumber(127), Eq(bmmidi::Control::k127));
  EXPECT_THAT(bmmidi::controlToNumber(bmmidi::Control::k127), Eq(127));
  EXPECT_THAT(bmmidi::controlToNumber(bmmidi::Control::kPolyMode), Eq(127));
}

TEST(Control, ShouldConvertToAndFromDataValue) {
  EXPECT_THAT(bmmidi::controlFromDataValue(bmmidi::DataValue{0}),
              Eq(bmmidi::Control::k000));
  EXPECT_THAT(bmmidi::controlToDataValue(bmmidi::Control::k000),
              Eq(bmmidi::DataValue{0}));
  EXPECT_THAT(bmmidi::controlToDataValue(bmmidi::Control::kBankSelect),
              Eq(bmmidi::DataValue{0}));

  EXPECT_THAT(bmmidi::controlFromDataValue(bmmidi::DataValue{1}),
              Eq(bmmidi::Control::k001));
  EXPECT_THAT(bmmidi::controlToDataValue(bmmidi::Control::k001),
              Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(bmmidi::controlToDataValue(bmmidi::Control::kModWheel),
              Eq(bmmidi::DataValue{1}));

  EXPECT_THAT(bmmidi::controlFromDataValue(bmmidi::DataValue{11}),
              Eq(bmmidi::Control::k011));
  EXPECT_THAT(bmmidi::controlToDataValue(bmmidi::Control::k011),
              Eq(bmmidi::DataValue{11}));
  EXPECT_THAT(bmmidi::controlToDataValue(bmmidi::Control::kExpression),
              Eq(bmmidi::DataValue{11}));

  EXPECT_THAT(bmmidi::controlFromDataValue(bmmidi::DataValue{126}),
              Eq(bmmidi::Control::k126));
  EXPECT_THAT(bmmidi::controlToDataValue(bmmidi::Control::k126),
              Eq(bmmidi::DataValue{126}));
  EXPECT_THAT(bmmidi::controlToDataValue(bmmidi::Control::kMonoMode),
              Eq(bmmidi::DataValue{126}));

  EXPECT_THAT(bmmidi::controlFromDataValue(bmmidi::DataValue{127}),
              Eq(bmmidi::Control::k127));
  EXPECT_THAT(bmmidi::controlToDataValue(bmmidi::Control::k127),
              Eq(bmmidi::DataValue{127}));
  EXPECT_THAT(bmmidi::controlToDataValue(bmmidi::Control::kPolyMode),
              Eq(bmmidi::DataValue{127}));
}

}  // namespace
