#ifndef __XNODE_H__
#define __XNODE_H__

struct xnode_module {
    int nm_version;
    unsigned int nm_flags;
    void* nm_dso_handle;
    const char* nm_filename;
    //
    const char* nm_modname;
    struct xnode_module* nm_link;
};

extern "C" void xnode_module_register(void* mod);

#endif