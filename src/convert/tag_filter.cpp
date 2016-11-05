#include "tag_filter.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <iomanip>
#include <fstream>

#include "tag_filter_config_parser.hpp"
#include "geometry.hpp"

using namespace std ;

bool TagFilter::parse(const string &fileName)
{
    ifstream strm(fileName.c_str()) ;
    if ( !strm ) {
        cerr << "Cannot open config file: " << fileName << endl ;
        return false ;
    }

    TagFilterConfigParser parser(strm, lua_) ;

    if ( ! parser.parse() )  {
        cerr << "Error parsing " << fileName << endl ;
        cerr << parser.error_string_ << endl ;
        return false ;
    }

    rules_ = parser.rules_ ;
    if ( !lua_.loadScript(parser.script_) ) {
        cerr << lua_.lastError() << endl ;
        return false ;
    }

    return true ;
}

// find the zoom range of attached tags

static bool normalize_tags(TagWriteList &tags, uint8_t &minz, uint8_t &maxz) {
    uint n_matched_tags = 0 ;
    minz = 255 ; maxz = 0;

    for( TagWriteAction &a: tags.actions_ ) {
        if ( !a.attached_ ) {
            minz = std::min(minz, a.zoom_min_) ;
            maxz = std::max(maxz, a.zoom_max_) ;
            n_matched_tags ++ ;
        }
    }

    if ( n_matched_tags == 0 ) return false ;

    for( TagWriteAction &a: tags.actions_ ) {
        if ( a.attached_ ) {
            a.zoom_min_ = minz ;
            a.zoom_max_ = maxz ;
        }
    }

    return true ;
}


bool TagFilter::matchRule(const tag_filter::RulePtr &rule, TagFilterContext &ctx, bool &cont)
{
    using namespace tag_filter ;

    cont = false ;

    if ( !rule->condition_ || rule->condition_->eval(ctx).toBoolean() ) {

        bool rcont = true ;

        for ( const CommandPtr &cmd: rule->commands_ ) {
            if ( cmd->type() == Command::Conditional && rcont ) {
                RuleCommand *rc = dynamic_cast<RuleCommand *>(cmd.get());
                bool c = false ;
                if ( matchRule(rc->rule_, ctx, c) ) {
                    if ( !c ) rcont = false ;
                }
            }
            else if ( cmd->type() == Command::Set ) {
                SimpleCommand *rc = dynamic_cast<SimpleCommand *>(cmd.get());

                string val = rc->val_->eval(ctx).toString() ;

                if ( !val.empty() ) {
                    if ( ctx.has_tag(rc->tag_ ) )
                        ctx.tags_[rc->tag_] = val ;
                    else
                        ctx.tags_.add(rc->tag_, val) ;
                }
            }
            else if ( cmd->type() == Command::Add ) {
                SimpleCommand *rc = dynamic_cast<SimpleCommand *>(cmd.get());
                string val = rc->val_->eval(ctx).toString() ;
                if ( !val.empty() ) ctx.tags_.add(rc->tag_, val) ;
            }
            else if ( cmd->type() == Command::Continue ) {
                cont = true ;
            }
            else if ( cmd->type() == Command::Delete ) {
                SimpleCommand *rc = dynamic_cast<SimpleCommand *>(cmd.get());
                ctx.tags_.remove(rc->tag_) ;
            }
            else if ( cmd->type() == Command::WriteAll ) {
                WriteAllCommand *rc = dynamic_cast<WriteAllCommand *>(cmd.get());
                ZoomRange zr = rc->zoom_range_ ;
                DictionaryIterator it(ctx.tags_) ;
                while ( it ) {
                    string val = it.value() ;
                    ctx.tw_.actions_.emplace_back(it.key(), val, zr.min_zoom_, zr.max_zoom_) ;
                    ++it ;
                }
            }
            else if ( cmd->type() == Command::Write ) {
                WriteCommand *rc = dynamic_cast<WriteCommand *>(cmd.get());
                ZoomRange zr = rc->zoom_range_ ;
                for( const TagDeclarationPtr &decl: rc->tags_.tags_ ) {
                    if ( ctx.has_tag(decl->tag_) ) {
                        if ( decl->val_ ) {
                            string val = decl->val_->eval(ctx).toString() ;
                            if ( !val.empty() )
                                ctx.tw_.actions_.emplace_back(decl->tag_, val, zr.min_zoom_, zr.max_zoom_) ;
                        }
                        else
                            ctx.tw_.actions_.emplace_back(decl->tag_, ctx.value(decl->tag_), zr.min_zoom_, zr.max_zoom_) ;
                    }
                }
            }
            else if ( cmd->type() == Command::Exclude ) {
                ExcludeCommand *rc = dynamic_cast<ExcludeCommand *>(cmd.get());

                set<string> exclude(rc->tags_.begin(), rc->tags_.end()) ;

                ZoomRange zr = rc->zoom_range_ ;
                DictionaryIterator it(ctx.tags_) ;
                while ( it ) {
                    if ( exclude.count(it.key()) == 0 )
                        ctx.tw_.actions_.emplace_back(it.key(), it.value(), zr.min_zoom_, zr.max_zoom_) ;
                    ++it ;
                }
            }
            else if ( cmd->type() == Command::Attach ) {
                AttachCommand *rc = dynamic_cast<AttachCommand *>(cmd.get());

                set<string> tags(rc->tags_.tags_.begin(), rc->tags_.tags_.end()) ;

                DictionaryIterator it(ctx.tags_) ;
                while ( it ) {
                    if ( tags.count(it.key()) )
                        ctx.tw_.actions_.emplace_back(it.key(), it.value(), 0, 255, true) ;
                    ++it ;
                }
            }
            else if ( cmd->type() == Command::UserFunction ) {
                FunctionCommand *rc = dynamic_cast<FunctionCommand *>(cmd.get());

                rc->func_->eval(ctx) ;
            }
        }
        return true ;
    }
    else return false ;
}

bool TagFilter::match(TagFilterContext &ctx, uint8_t &zmin, uint8_t &zmax) {

    using namespace tag_filter ;

    lua_.setupContext(ctx) ;

    bool cont = false;

    for( const RulePtr &rule: rules_ ) {
        if ( matchRule( rule, ctx, cont) ) {
            if ( !cont ) break ;
        }
    }

    return normalize_tags(ctx.tw_, zmin, zmax)  ;
}
