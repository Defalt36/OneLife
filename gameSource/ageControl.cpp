#include "ageControl.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/stringUtils.h"

#include <math.h>


static double babyHeadDownFactor = 0.6;

static double babyBodyDownFactor = 0.75;

static double oldHeadDownFactor = 0.35;

static double oldHeadForwardFactor = 2;

static SimpleVector<doublePair> ageScaling;

static doublePair defaultAgeScaling[] = {
  {20, 20},
  {104, 40},
  {117, 57},
  {120, 60}
};

#include "minorGems/util/SettingsManager.h"

float lifespan_multiplier = 1.0f;
int age_baby = 5;
int age_fertile = 14;
int age_mature = 20;
int age_old = (int)( 40 * lifespan_multiplier );
int age_death = (int)( 60 * lifespan_multiplier );


void sanityCheckSettings() {
    FILE *fp = SettingsManager::getSettingsFile( "lifespanMultiplier", "r" );
	if( fp == NULL ) {
		fp = SettingsManager::getSettingsFile( "lifespanMultiplier", "w" );
        SettingsManager::setSetting( "lifespanMultiplier", 1.0f );
	}
    fclose( fp );
}


void initAgeControl() {
    babyHeadDownFactor = 
        SettingsManager::getFloatSetting( "babyHeadDownFactor", 0.6 );
    
    babyBodyDownFactor = 
        SettingsManager::getFloatSetting( "babyBodyDownFactor", 0.75 );

    oldHeadDownFactor = 
        SettingsManager::getFloatSetting( "oldHeadDownFactor", 0.35 );

    oldHeadForwardFactor = 
        SettingsManager::getFloatSetting( "oldHeadForwardFactor", 2 );

    initAgeScaling();
    
    sanityCheckSettings();
    lifespan_multiplier = SettingsManager::getFloatSetting( "lifespanMultiplier", 1.0f );
    age_old = (int)( 40 * lifespan_multiplier );
    age_death = (int)( 60 * lifespan_multiplier );
    }




doublePair getAgeHeadOffset( double inAge, doublePair inHeadSpritePos,
                             doublePair inBodySpritePos,
                             doublePair inFrontFootSpritePos ) {
    if( inAge == -1 ) {
        return (doublePair){ 0, 0 };
        }
    
    if( inAge < age_mature ) {
        
        double maxHead = inHeadSpritePos.y - inBodySpritePos.y;
        
        double yOffset = ( ( age_mature - inAge ) / age_mature ) * babyHeadDownFactor * maxHead;
        
        
        return (doublePair){ 0, round( -yOffset ) };
        }
    

    if( inAge >= age_old ) {
        
        if( inAge > age_death ) {
            // no worse after age_death
            inAge = age_death;
            }

        double maxHead = inHeadSpritePos.y - inBodySpritePos.y;
        
        double vertOffset = 
            ( ( inAge - age_old ) / age_mature ) * oldHeadDownFactor * maxHead;
        
        double footOffset = inFrontFootSpritePos.x - inHeadSpritePos.x;
        
        double forwardOffset = 
            ( ( inAge - age_old ) / age_mature ) * oldHeadForwardFactor * footOffset;

        return (doublePair){ round( forwardOffset ), round( -vertOffset ) };
        }

    return (doublePair){ 0, 0 };
    }



doublePair getAgeBodyOffset( double inAge, doublePair inBodySpritePos ) {
    if( inAge == -1 ) {
        return (doublePair){ 0, 0 };
        }
    
    if( inAge < age_mature ) {
        
        double maxBody = inBodySpritePos.y;
        
        double yOffset = ( ( age_mature - inAge ) / age_mature ) * babyBodyDownFactor * maxBody;
        
        
        return (doublePair){ 0, round( -yOffset ) };
        }

    return (doublePair){ 0, 0 };
    }

double fakeStartNextAgeFileRead( double inAge );
void fakeRestartMusic( double inAge, double inAgeRate, char inForceNow );

void testAgeScaling() {
    printf("s%lf -> d%lf\n", 104.0, computeDisplayAge(104));
    printf("s%lf -> d%lf\n", 117.0, computeDisplayAge(117));
    printf("s%lf -> d%lf\n", 115.0, computeDisplayAge(115));
    printf("s%lf -> d%lf\n", 121.1, computeDisplayAge(121.1));

    printf("d%lf -> s%lf\n", 40.0, computeServerAge(40));
    printf("d%lf -> s%lf\n", 57.0, computeServerAge(57));
    printf("d%lf -> s%lf\n", 55.0, computeServerAge(55));
    printf("d%lf -> s%lf\n", 50.0, computeServerAge(50));
    printf("d%lf -> s%lf\n", 60.1, computeServerAge(60.1));
    }


void initAgeScaling() {
    char *cont = SettingsManager::getSettingContents( "ageScaling", "" );

    ageScaling.deleteAll();

    if( strcmp( cont, "" ) == 0 ) {
        delete [] cont;
        for( size_t i = 0;i < sizeof(defaultAgeScaling); i++) {
            ageScaling.push_back( defaultAgeScaling[i] );
            }
        return;
        }

    int numParts;
    char **parts = split( cont, "\n", &numParts );
    delete [] cont;

    for( int i=0; i<numParts; i++ ) {
        if( strcmp( parts[i], "" ) != 0 ) {

            float serverAge;
            float displayAge;
            int numRead = sscanf( parts[i], "%f %f",
                                  &serverAge, &displayAge );
            if( numRead == 2 ) {
                doublePair point = { serverAge, displayAge };
                printf("age scale %lf %lf\n", point.x, point.y);
                ageScaling.push_back( point );
                }
            }
        delete [] parts[i];
        }
    delete [] parts;

    //testAgeScaling();
    }


double computeDisplayAge( double serverAge ) {
    doublePair start = {0,0};
    for( int i = 0;i < ageScaling.size();i++ ) {
        doublePair end = ageScaling.getElementDirect(i);

        if( start.x <= serverAge && serverAge < end.x ){
            double serverDelta = serverAge - start.x;
            double serverRange = end.x - start.x;
            double fraction;
            if( serverRange == 0 ) {
                fraction = 0;
                }
            else {
                fraction = serverDelta / serverRange;
                }
            double displayRange = end.y - start.y;
            double displayAge = start.y + displayRange*fraction;
            return displayAge;
            }

        start = end;
        }
    return serverAge - start.x + start.y;
    }

double computeServerAge( double displayAge ) {
    doublePair start = {0,0};
    for( int i = 0;i < ageScaling.size();i++ ) {
        doublePair end = ageScaling.getElementDirect(i);

        if( start.y <= displayAge && displayAge < end.y ){
            double displayDelta = displayAge - start.y;
            double displayRange = end.y - start.y;
            double fraction;
            if( displayRange == 0 ) {
                fraction = 0;
                }
            else {
                fraction = displayDelta / displayRange;
                }
            double serverRange = end.x - start.x;
            double serverAge = start.x + serverRange*fraction;
            return serverAge;
            }

        start = end;
        }
    return displayAge - start.y + start.x;
    }
