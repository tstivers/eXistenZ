/////////////////////////////////////////////////////////////////////////////
// common.h
// contains random crap
// $Id: common.h,v 1.1 2003/10/09 02:47:03 tstivers Exp $
//

#pragma once

void load_alias_list(const char* filename, alias_list& list);
char* find_alias(const char* key, alias_list& list);
