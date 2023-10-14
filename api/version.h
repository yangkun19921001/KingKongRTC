//
// Created by devyk on 2023/9/18.
//

#ifndef KKRTC_VERSION_H
#define KKRTC_VERSION_H

#define KKRTC_MAJOR_VERSION 1
#define KKRTC_MINOR_VERSION 0
#define KKRTC_PATCH_VERSION 0


#define KKAUX_STR_EXP(__A)  #__A
#define KKAUX_STR(__A)      KKAUX_STR_EXP(__A)

#define KKAUX_STRW_EXP(__A)  L ## #__A
#define KKAUX_STRW(__A)      KKAUX_STRW_EXP(__A)

#define KK_VERSION          KKAUX_STR(KKRTC_MAJOR_VERSION) "." KKAUX_STR(KKRTC_MINOR_VERSION) "." KKAUX_STR(KKRTC_PATCH_VERSION)

#endif //KKRTC_VERSION_H
