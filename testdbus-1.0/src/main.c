#include "testdbus-generated.h"
#include <stdio.h>

static GMainLoop *loop = NULL;
static gboolean opt_replace = FALSE;
static gboolean opt_no_debug = FALSE;
static gboolean opt_no_sigint = FALSE;
static GOptionEntry opt_entries[] =
{
    {"replace", 'r', 0, G_OPTION_ARG_NONE, &opt_replace, "Replace existing daemon", NULL},
    {"no-debug", 'n', 0, G_OPTION_ARG_NONE, &opt_no_debug, "Don't print debug information on stdout/stderr", NULL},
    {"no-sigint", 's', 0, G_OPTION_ARG_NONE, &opt_no_sigint, "Do not handle SIGINT for controlled shutdown", NULL},
    {NULL }
};

static TestdbusBase *skeleton = NULL;

static gboolean on_handle_test_int(TestdbusBase* skeleton,
                                                    GDBusMethodInvocation *invocation,
                                                    gpointer user_data)
{
    g_printerr ("Method call:on_handle_test_int\n");
    testdbus_base_complete_test_int(skeleton, invocation, 55);

    return TRUE;
}

static gboolean on_handle_test_structs(TestdbusBase* skeleton,
                                                    GDBusMethodInvocation *invocation,
                                                    gpointer user_data)
{
    GVariantBuilder builder;
    GVariant *ret = NULL;

    g_printerr ("Method call:on_handle_test_structs\n");
    g_variant_builder_init (&builder, G_VARIANT_TYPE ("a(si)"));
    g_variant_builder_add (&builder, "(si)", "test1", 1);
    g_variant_builder_add (&builder, "(si)", "test2", 2);
    g_variant_builder_add (&builder, "(si)", "test3", 3);
    ret = g_variant_builder_end (&builder);
    testdbus_base_complete_test_structs(skeleton, invocation, ret);
    return TRUE;
}

static gboolean on_handle_test_str(TestdbusBase* skeleton,
                                                    GDBusMethodInvocation *invocation,
                                                    gint index,
                                                    gpointer user_data)
{
    g_printerr ("Method call:on_handle_test_str\n");
    char *reply1 = "hello world";
    char *reply2 = "hello dbus";
    if (index == 1) {
        testdbus_base_complete_test_str(skeleton, invocation, reply1);
    } else if (index == 2) {
        testdbus_base_complete_test_str(skeleton, invocation, reply2);
    } else {
        testdbus_base_complete_test_str(skeleton, invocation, "error");
    }

    return TRUE;
}


static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 gpointer         user_data)
{
    GError *error = NULL;

    skeleton = testdbus_base_skeleton_new ();
    g_signal_connect(skeleton, "handle-test-int", G_CALLBACK(on_handle_test_int), NULL);
    g_signal_connect(skeleton, "handle-test-str", G_CALLBACK(on_handle_test_str), NULL);
    g_signal_connect(skeleton, "handle-test-structs", G_CALLBACK(on_handle_test_structs), NULL);
    g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(skeleton), connection,
                                                        "/org/freedesktop/Testdbus/Base", &error);

    if (error != NULL) {
        g_printerr("Error:failed to export object. Reason:%s\n", error->message);
        g_error_free(error);
        return;
    }
    g_printerr ("Connected to the system bus\n");
}

static void
on_name_lost (GDBusConnection *connection,
              const gchar     *name,
              gpointer         user_data)
{
    g_printerr ("Lost (or failed to acquire) the name %s on the system message bus\n", name);
    g_main_loop_quit (loop);
}

static void
on_name_acquired (GDBusConnection *connection,
                  const gchar     *name,
                  gpointer         user_data)
{
    g_printerr ("Acquired the name %s on the system message bus\n", name);
}

static gboolean
on_sigint (gpointer user_data)
{
    g_main_loop_quit (loop);
    return FALSE;
}

int main(int argc, char **argv)
{
    GError *error;
    GOptionContext *opt_context;
    gint ret;
    guint name_owner_id;
    guint sigint_id;

    ret = 1;
    loop = NULL;
    opt_context = NULL;
    name_owner_id = 0;
    sigint_id = 0;

    //g_type_init ();
    
    opt_context = g_option_context_new ("test dbus daemon");
    g_option_context_add_main_entries (opt_context, opt_entries, NULL);
    error = NULL;
    if (!g_option_context_parse (opt_context, &argc, &argv, &error))
    {
        g_printerr ("Error parsing options: %s\n", error->message);
        g_error_free (error);
        goto out;
    }

    loop = g_main_loop_new (NULL, FALSE);
    sigint_id = 0;
    if (!opt_no_sigint)
    {
        sigint_id = g_unix_signal_add_full (G_PRIORITY_DEFAULT,
                                            SIGINT,
                                            on_sigint,
                                            NULL,  /* user_data */
                                            NULL); /* GDestroyNotify */
    }
    name_owner_id = g_bus_own_name (G_BUS_TYPE_SYSTEM,
                                  "org.freedesktop.Testdbus",
                                  G_BUS_NAME_OWNER_FLAGS_NONE,
                                  on_bus_acquired,
                                  on_name_acquired,
                                  on_name_lost,
                                  NULL,
                                  NULL);

    g_main_loop_run (loop);
    ret = 0;

out:
    if (sigint_id > 0)
        g_source_remove (sigint_id);
    if (skeleton != NULL)
        g_object_unref (skeleton);
    if (name_owner_id != 0)
        g_bus_unown_name (name_owner_id);
    if (loop != NULL)
        g_main_loop_unref (loop);
    if (opt_context != NULL)
        g_option_context_free (opt_context);

    return ret;
}
