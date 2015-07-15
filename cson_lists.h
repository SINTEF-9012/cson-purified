/* Auto-generated from cson_list.h. Edit at your own risk! */
unsigned int cson_value_list_reserve( cson_value_list * self, unsigned int n )
{
    if( !self ) return 0;
    else if(0 == n)
    {
        if(0 == self->alloced) return 0;
        cson_free(self->list, "cson_value_list_reserve");
        self->list = NULL;
        self->alloced = self->count = 0;
        return 0;
    }
    else if( self->alloced >= n )
    {
        return self->alloced;
    }
    else
    {
        size_t const sz = sizeof(cson_value *) * n;
        cson_value * * m = (cson_value **)cson_realloc( self->list, sz, "cson_value_list_reserve" );
        if( ! m ) return self->alloced;

        memset( m + self->alloced, 0, (sizeof(cson_value *)*(n-self->alloced)));
        self->alloced = n;
        self->list = m;
        return n;
    }
}
int cson_value_list_append( cson_value_list * self, cson_value * cp )
{
    if( !self || !cp ) return cson_rc.ArgError;
    else if( self->alloced > cson_value_list_reserve(self, self->count+1) )
    {
        return cson_rc.AllocError;
    }
    else
    {
        self->list[self->count++] = cp;
        return 0;
    }
}
int cson_value_list_visit( cson_value_list * self,

                        int (*visitor)(cson_value * obj, void * visitorState ),



                        void * visitorState )
{
    int rc = cson_rc.ArgError;
    if( self && visitor )
    {
        unsigned int i = 0;
        for( rc = 0; (i < self->count) && (0 == rc); ++i )
        {

            cson_value * obj = self->list[i];



            if(obj) rc = visitor( obj, visitorState );
        }
    }
    return rc;
}
void cson_value_list_clean( cson_value_list * self,

                         void (*cleaner)(cson_value * obj)



                         )
{
    if( self && cleaner && self->count )
    {
        unsigned int i = 0;
        for( ; i < self->count; ++i )
        {

            cson_value * obj = self->list[i];



            if(obj) cleaner(obj);
        }
    }
    cson_value_list_reserve(self,0);
}
unsigned int cson_kvp_list_reserve( cson_kvp_list * self, unsigned int n )
{
    if( !self ) return 0;
    else if(0 == n)
    {
        if(0 == self->alloced) return 0;
        cson_free(self->list, "cson_kvp_list_reserve");
        self->list = NULL;
        self->alloced = self->count = 0;
        return 0;
    }
    else if( self->alloced >= n )
    {
        return self->alloced;
    }
    else
    {
        size_t const sz = sizeof(cson_kvp *) * n;
        cson_kvp * * m = (cson_kvp **)cson_realloc( self->list, sz, "cson_kvp_list_reserve" );
        if( ! m ) return self->alloced;

        memset( m + self->alloced, 0, (sizeof(cson_kvp *)*(n-self->alloced)));
        self->alloced = n;
        self->list = m;
        return n;
    }
}
int cson_kvp_list_append( cson_kvp_list * self, cson_kvp * cp )
{
    if( !self || !cp ) return cson_rc.ArgError;
    else if( self->alloced > cson_kvp_list_reserve(self, self->count+1) )
    {
        return cson_rc.AllocError;
    }
    else
    {
        self->list[self->count++] = cp;
        return 0;
    }
}
int cson_kvp_list_visit( cson_kvp_list * self,

                        int (*visitor)(cson_kvp * obj, void * visitorState ),



                        void * visitorState )
{
    int rc = cson_rc.ArgError;
    if( self && visitor )
    {
        unsigned int i = 0;
        for( rc = 0; (i < self->count) && (0 == rc); ++i )
        {

            cson_kvp * obj = self->list[i];



            if(obj) rc = visitor( obj, visitorState );
        }
    }
    return rc;
}
void cson_kvp_list_clean( cson_kvp_list * self,

                         void (*cleaner)(cson_kvp * obj)



                         )
{
    if( self && cleaner && self->count )
    {
        unsigned int i = 0;
        for( ; i < self->count; ++i )
        {

            cson_kvp * obj = self->list[i];



            if(obj) cleaner(obj);
        }
    }
    cson_kvp_list_reserve(self,0);
}
