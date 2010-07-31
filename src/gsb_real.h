#ifndef _GSB_REAL_H
#define _GSB_REAL_H (1)


/** \struct
 *  Describe a real number.
 * */
typedef struct
{
    glong mantissa;
    gint exponent;
} gsb_real;


/* START_INCLUDE_H */
/* END_INCLUDE_H */

/* START_DECLARATION */
gsb_real gsb_real_abs ( gsb_real number );
G_MODULE_EXPORT gsb_real gsb_real_add ( gsb_real number_1,
                        gsb_real number_2 );
gsb_real gsb_real_adjust_exponent ( gsb_real number,
                        gint return_exponent );
G_MODULE_EXPORT gint gsb_real_cmp ( gsb_real number_1,
                        gsb_real number_2 );
gsb_real gsb_real_div ( gsb_real number_1,
                        gsb_real number_2 );
G_MODULE_EXPORT gsb_real gsb_real_double_to_real ( gdouble number );
gchar *gsb_real_format_string ( gsb_real number,
                        gint currency_number,
                        gboolean show_symbol );
gsb_real gsb_real_get_from_string ( const gchar *string );
gchar *gsb_real_get_string ( gsb_real number );
gchar *gsb_real_get_string_with_currency ( gsb_real number,
                        gint currency_number,
                        gboolean show_symbol );
gsb_real gsb_real_import_from_string ( const gchar *string );
gsb_real gsb_real_mul ( gsb_real number_1,
                        gsb_real number_2 );
gsb_real gsb_real_new ( gint mantissa, gint exponent );
gboolean gsb_real_normalize ( gsb_real *number_1, gsb_real *number_2 );
G_MODULE_EXPORT gsb_real gsb_real_opposite ( gsb_real number );
gchar *gsb_real_raw_format_string (gsb_real number,
                        struct lconv *conv,
                        const gchar *currency_symbol );
gsb_real gsb_real_raw_get_from_string ( const gchar *string,
                                        const gchar *mon_thousands_sep,
                                        const gchar *mon_decimal_point );
gdouble gsb_real_real_to_double ( gsb_real number );
gchar *gsb_real_save_real_to_string ( gsb_real number, gint default_exponent );
gsb_real gsb_real_sub ( gsb_real number_1,
                        gsb_real number_2 );
gsb_real gsb_str_to_real ( const gchar * str );
/* END_DECLARATION */
#endif
