/*******************************************************************************
 * Proj: Component's info facility.                                            *
 * --------------------------------------------------------------------------- *
 * File: component_info.h                                                      *
 * Created: Wed Dec 10 20:34:40 2003                                           *
 * Desc: Functions prototypes for component's information.                     *
 * --------------------------------------------------------------------------- *
 * Andrey L. Kalinin, andrey@kalinin.ru                                        *
 *******************************************************************************/

/**
 * \file  component_info.h
 * \brief Functions prototypes for component's information.
 */

#ifndef __component_info_h__
#define __component_info_h__

#ifdef __cplusplus
extern "C"
{
#endif

    /* printf-like callback */
    typedef int (* component_info_callback)(void *context, const char *format, ... );

    /*
     * All components have to get function like
     * int component_build_info(void *context, component_info_callback cic);
     * 
     * which will print build information for this component.
     */

#ifdef __cplusplus
}
#endif

#endif /* __component_info_h__ */

/*
 * <eof component_info.h>
 */
