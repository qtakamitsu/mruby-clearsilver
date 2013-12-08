#include <mruby.h>
#include <mruby/proc.h>
#include <mruby/compile.h>
#include <mruby/data.h>
#include <mruby/class.h>
#include <mruby/string.h>
#include <stdio.h>
#include <ClearSilver/ClearSilver.h>

// HDF
static void mrb_hdf_free(mrb_state *mrb, void *p)
{
    HDF *hdf = (HDF *)p;

    if (hdf != NULL) {
        hdf_destroy(&hdf);
    }
}

static const mrb_data_type mrb_hdf_type = {
  "mrb_clearsilver_hdf", mrb_hdf_free,
};

static mrb_value mrb_hdf_init(mrb_state *mrb, mrb_value self)
{
    HDF *hdf;

    hdf_init(&hdf);

    DATA_PTR(self) = hdf;
    DATA_TYPE(self) = &mrb_hdf_type;

    return self;
}

static mrb_value mrb_hdf_set_value(mrb_state *mrb, mrb_value self)
{
    mrb_value argv1;
    mrb_value argv2;
    int arg;

    arg = mrb_get_args(mrb, "SS", &argv1, &argv2);
    if (arg != 2) {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "argument");
    }

    HDF *hdf = mrb_get_datatype(mrb, self, &mrb_hdf_type);

    struct RString *name = mrb_str_ptr(argv1);
    struct RString *val = mrb_str_ptr(argv2);

    hdf_set_value(hdf, name->ptr, val->ptr);

    return mrb_fixnum_value(0);
}

static mrb_value mrb_hdf_get_value(mrb_state *mrb, mrb_value self)
{
    mrb_value argv1;
    mrb_value argv2;
    int arg;

    arg = mrb_get_args(mrb, "SS", &argv1, &argv2);
    if (arg != 2) {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "argument");
    }

    HDF *hdf = mrb_get_datatype(mrb, self, &mrb_hdf_type);

    struct RString *name = mrb_str_ptr(argv1);
    struct RString *def_val = mrb_str_ptr(argv2);

    char *value = hdf_get_value(hdf, name->ptr, def_val->ptr);

    return mrb_str_new(mrb, value, strlen(value));
}

static mrb_value mrb_hdf_remove_tree(mrb_state *mrb, mrb_value self)
{
    mrb_value argv1;
    int arg;

    arg = mrb_get_args(mrb, "S", &argv1);
    if (arg != 1) {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "argument");
    }

    HDF *hdf = mrb_get_datatype(mrb, self, &mrb_hdf_type);

    struct RString *name = mrb_str_ptr(argv1);

    hdf_remove_tree(hdf, name->ptr);

    return mrb_fixnum_value(0);
}

static mrb_value mrb_hdf_dump(mrb_state *mrb, mrb_value self)
{
    mrb_value argv1;
    int arg;
    char *prefix = NULL;

    arg = mrb_get_args(mrb, "|S", &argv1);
    if (arg == 1) {
        prefix = RSTRING_PTR(argv1);
    }

    HDF *hdf = mrb_get_datatype(mrb, self, &mrb_hdf_type);

    hdf_dump(hdf, prefix);

    return mrb_fixnum_value(0);
}

// CS
static void mrb_cs_free(mrb_state *mrb, void *p)
{
    CSPARSE *cs = (CSPARSE *)p;

    if (cs != NULL) {
        cs_destroy(&cs);
    }
}

static const mrb_data_type mrb_cs_type = {
  "mrb_clearsilver_cs", mrb_cs_free,
};

static mrb_value mrb_cs_init(mrb_state *mrb, mrb_value self)
{
    CSPARSE *cs;
    mrb_value argv1;
    int arg;

    arg = mrb_get_args(mrb, "o", &argv1);
    if (arg != 1) {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "argument");
    }

    HDF *hdf = mrb_get_datatype(mrb, argv1, &mrb_hdf_type);
    if (hdf == NULL) {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "argument");
    }

    cs_init(&cs, hdf);

    DATA_PTR(self) = cs;
    DATA_TYPE(self) = &mrb_cs_type;

    return self;
}

static mrb_value mrb_cs_parse_file(mrb_state *mrb, mrb_value self)
{
    mrb_value argv1;
    int arg;

    arg = mrb_get_args(mrb, "S", &argv1);
    if (arg != 1) {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "argument");
    }

    CSPARSE *cs = mrb_get_datatype(mrb, self, &mrb_cs_type);

    cs_parse_file(cs, RSTRING_PTR(argv1));

    return mrb_fixnum_value(0);
}

static mrb_value mrb_cs_parse_string(mrb_state *mrb, mrb_value self)
{
    mrb_value argv1;
    int arg;

    arg = mrb_get_args(mrb, "S", &argv1);
    if (arg != 1) {
        mrb_raise(mrb, E_ARGUMENT_ERROR, "argument");
    }

    CSPARSE *cs = mrb_get_datatype(mrb, self, &mrb_cs_type);

    cs_parse_string(cs, RSTRING_PTR(argv1), RSTRING_LEN(argv1));

    return mrb_fixnum_value(0);
}

NEOERR *render_string(void *ctx, char *s)
{
    STRING *out = (STRING *)ctx;

    string_append(out, s);

    return STATUS_OK;
}

static mrb_value mrb_cs_render(mrb_state *mrb, mrb_value self)
{
    CSPARSE *cs = mrb_get_datatype(mrb, self, &mrb_cs_type);

    STRING render_result;

    string_init(&render_result);

    cs_render(cs, &render_result, render_string);

    mrb_value ret = mrb_str_new(mrb, render_result.buf, render_result.len);

    string_clear(&render_result);

    return ret;
}

void mrb_mruby_clearsilver_gem_init(mrb_state* mrb)
{
    struct RClass *module_clearsilver;
    struct RClass *class_hdf;
    struct RClass *class_cs;

    module_clearsilver = mrb_define_module(mrb, "ClearSilver");

    class_hdf = mrb_define_class(mrb, "HDF", mrb->object_class);
    MRB_SET_INSTANCE_TT(class_hdf, MRB_TT_DATA);
    mrb_define_method(mrb, class_hdf, "initialize", mrb_hdf_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, class_hdf, "set_value", mrb_hdf_set_value, MRB_ARGS_OPT(2));
    mrb_define_method(mrb, class_hdf, "get_value", mrb_hdf_get_value, MRB_ARGS_OPT(2));
    mrb_define_method(mrb, class_hdf, "remove_tree", mrb_hdf_remove_tree, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, class_hdf, "dump", mrb_hdf_dump, MRB_ARGS_ANY());

    class_cs = mrb_define_class(mrb, "CS", mrb->object_class);
    MRB_SET_INSTANCE_TT(class_cs, MRB_TT_DATA);
    mrb_define_method(mrb, class_cs, "initialize", mrb_cs_init, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, class_cs, "parse_file", mrb_cs_parse_file, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, class_cs, "parse_string", mrb_cs_parse_string, MRB_ARGS_OPT(1));
    mrb_define_method(mrb, class_cs, "render", mrb_cs_render, MRB_ARGS_NONE());

    return;
}

void mrb_mruby_clearsilver_gem_final(mrb_state* mrb)
{
    // finalizer
}
