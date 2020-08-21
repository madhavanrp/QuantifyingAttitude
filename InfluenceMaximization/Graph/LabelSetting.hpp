//
//  LabelSetting.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 3/8/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#ifndef LabelSetting_hpp
#define LabelSetting_hpp

#include <stdio.h>

enum LabelSetting {
    LabelSettingUniform,
    LabelSettingClusters,
    // Find a seed set using TIM. Set all the nodes influenced by that to be non targets.
    LabelSettingTIMNonTargets
};

#endif /* LabelSetting_hpp */
