#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "testdbus-generated.h"


static gboolean opt_vi = FALSE;
static gboolean opt_vs = FALSE;
static gboolean opt_ss = FALSE;
static GOptionEntry opt_entries[] =
{
    {"voidint", 'i', 0, G_OPTION_ARG_NONE, &opt_vi, "no input, return int", NULL},
    {"voidstring", 's', 0, G_OPTION_ARG_NONE, &opt_vs, "no input, return string", NULL},
    {"voidstructs", 'S', 0, G_OPTION_ARG_NONE, &opt_ss, "no input, return structs", NULL},
    {NULL }
};

struct test_structs {
    char *ret_str;
    int ret_int;
};

int main (int argc, char **argv)
{
    TestdbusBase *proxy = NULL;
    GError *error = NULL;
    GOptionContext *opt_context = NULL;

    opt_context = g_option_context_new ("test gdbus");
    g_option_context_add_main_entries (opt_context, opt_entries, NULL);
    error = NULL;
    if (!g_option_context_parse (opt_context, &argc, &argv, &error))
    {
        g_printerr ("Error parsing options: %s\n", error->message);
        g_error_free (error);
        goto out;
    }

    if (opt_vi) {
        int ret;
        proxy = NULL;
        error = NULL;
        proxy = testdbus_base_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                                G_DBUS_PROXY_FLAGS_NONE,
                                                                "org.freedesktop.Testdbus",
                                                                "/org/freedesktop/Testdbus/Base",
                                                                NULL,
                                                                &error);
        if (proxy == NULL) {
            g_printerr ("Failed to create proxy: %s\n", error->message);
        }
        testdbus_base_call_test_int_sync(proxy, &ret, NULL, &error);
        g_printerr("sync reply:%d\n", ret);
    }

    if (opt_vs) {
        char *ret_str;
        proxy = NULL;
        error = NULL;
        proxy = testdbus_base_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                                G_DBUS_PROXY_FLAGS_NONE,
                                                                "org.freedesktop.Testdbus",
                                                                "/org/freedesktop/Testdbus/Base",
                                                                NULL,
                                                                &error);
        if (proxy == NULL) {
            g_printerr ("Failed to create proxy: %s\n", error->message);
        }
        testdbus_base_call_test_str_sync(proxy, 1, &ret_str, NULL, &error);
        g_printerr("sync reply:%s\n", ret_str);
        if (ret_str != NULL)
            g_free(ret_str);
    }

    if (opt_ss) {
        GVariant *ret = NULL;
        GVariant *child;
        GVariantIter iter;
        int count = 0, i = 0;
        proxy = NULL;
        error = NULL;
        gchar *ret_str;
        gint ret_int;
        proxy = testdbus_base_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                                G_DBUS_PROXY_FLAGS_NONE,
                                                                "org.freedesktop.Testdbus",
                                                                "/org/freedesktop/Testdbus/Base",
                                                                NULL,
                                                                &error);
        if (proxy == NULL) {
            g_printerr ("Failed to create proxy: %s\n", error->message);
        }
        testdbus_base_call_test_structs_sync(proxy, &ret, NULL, &error);
        g_variant_iter_init(&iter, ret);
        while (g_variant_iter_next(&iter, "(si)", &ret_str, &ret_int)) {
            g_printerr("sync reply:%s\n", ret_str);
            g_printerr("sync reply:%d\n", ret_int);
            g_free(ret_str);
        }
    }

out:
    if (opt_context != NULL)
        g_option_context_free (opt_context);
    if (proxy != NULL)
        g_object_unref(proxy);
    return 0;
}
