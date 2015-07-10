/*
 * Implementação da classe Player.
 *
 * Autor: Edson Alves
 * Data: 15/05/2015
 * Licença: LGPL. Sem copyright.
 */
#include "player.h"
#include "core/rect.h"
#include "core/level.h"
#include "core/environment.h"
#include "core/keyboardevent.h"

ActionID Player::hitExitDoorID { "hitExitDoorID()" };
//ActionID Player::hitKeyID{"hitKeyID()"};

using std::make_pair;

class Player::Impl
{
public:
    Impl(Player *player, Map *current_map, bool key)
        : m_player(player), m_direction(Player::LEFT),
        m_moviment(make_pair(0.0, 0.0)), m_current_map(current_map), 
        m_key(key), m_strength(0.0), m_life(100.0), m_sanity(100.0)
    {
    }

    Direction direction() const { return m_direction; }
    void set_direction(Direction direction) { m_direction = direction; }

    const pair<double, double>& moviment() const { return m_moviment; }

    void set_moviment(double xaxis, double yaxis)
    {
        m_moviment = make_pair(xaxis, yaxis);
    }

    void set_current(Room *nova, int posx, int posy)
    {
        m_player->set_x(posx);
        m_player->set_y(posy);
        m_current_map->set_current(nova);
    }

    void set_strength(double strength)
    {
        m_strength = strength;
    }

    double strength()
    {
        return m_strength;
    }

    void set_life(double life)
    {
        m_life = life;
    }

    double life()
    {
        return m_life;
    }

    void set_sanity(double sanity)
    {
        m_sanity = sanity;
    }

    double sanity()
    {
        return m_sanity;
    }

    void get_key()
    {
        if(m_key == true)
            return;
        
        Environment *env = Environment::get_instance();
        double size = env->canvas->w()/35;

        m_key = true;

        Item* key = new Item(m_player, "icon_key", "res/items/key.png", (double)env->canvas->w() * 1/35 + size, (double)env->canvas->h() * 25/30 + size, 9999, true);
        m_player->add_child(key);
    }
    void set_key(bool input)
    {
        m_key = input;
    }
    bool has_key()
    {
        return m_key;
    }
    void show_life()
    {
        Environment *env = Environment::get_instance();
        Rect healthbar {(double)env->canvas->w()/15, (double)env->canvas->h()/18, m_player->life()*2, 12};
        env->canvas->fill(healthbar, Color::RED);
    }
    void show_sanity()
    {
        Environment *env = Environment::get_instance();
        Rect sanitybar {(double)env->canvas->w()/15, (double)env->canvas->h()/13, m_player->sanity()*2, 12};
        env->canvas->fill(sanitybar, Color::GREEN);
    }

    void show_inventory()
    {
        Environment *env = Environment::get_instance();
        double size = env->canvas->w()/35;

        Rect item1 {(double)(env->canvas->w() * 1/35), (double)env->canvas->h() * 25/30, size, size};
        Rect item2 {(double)env->canvas->w() * 1/35 + size, (double)env->canvas->h() * 25/30 - size, size, size};
        Rect item3 {(double)env->canvas->w() * 1/35 + 2*size, (double)env->canvas->h() * 25/30, size, size};
        Rect item4 {(double)env->canvas->w() * 1/35 + size, (double)env->canvas->h() * 25/30 + size, size, size};
        Rect not_item {(double)env->canvas->w() * 1/35 + size, (double)env->canvas->h() * 25/30, size, size};
        env->canvas->draw(item1, Color::WHITE);
        env->canvas->draw(item2, Color::WHITE);
        env->canvas->draw(item3, Color::WHITE);
        env->canvas->draw(item4, Color::WHITE);
        env->canvas->fill(not_item, Color::WHITE);
    }

private:
    Player *m_player;
    Direction m_direction;
    pair<double, double> m_moviment;
    Map *m_current_map;
    bool m_key;
    double m_strength;
    double m_life;
    double m_sanity;
};

class Idle : public SpriteState
{
public:
    Idle(Player *player)
        : m_player(player), m_animation(new Animation("res/sprites/idle.png",
            0, 0, 70, 70, 2, 1000, true)), m_left(0), m_right(0), m_top(0), 
        m_down(0)
    {
    }

    ~Idle() {}

    void enter(int)
    {
        m_player->set_dimensions(m_animation->w(), m_animation->h());
        m_right = m_left = m_down = m_top = 0;
    }

    void leave(int)
    {
    }

    bool on_event(const KeyboardEvent& event)
    {
        switch (event.state())
        {
        case KeyboardEvent::PRESSED:
            switch (event.key())
            {
            case KeyboardEvent::LEFT:
                m_left = 1;
                return true;

            case KeyboardEvent::RIGHT:
                m_right = 1;
                return true;

            case KeyboardEvent::UP:
                m_top = 1;
                return true;

            case KeyboardEvent::DOWN:
                m_down = 1;
                return true;

            default:
                break;
            }
            break;

        case KeyboardEvent::RELEASED:
            switch (event.key())
            {
            case KeyboardEvent::LEFT:
                m_left = 0;
                return true;

            case KeyboardEvent::RIGHT:
                m_right = 0;
                return true;

            case KeyboardEvent::UP:
                m_top = 0;
                return true;

            case KeyboardEvent::DOWN:
                m_down = 0;
                return true;

            default:
                break;
            }
            break;
        }

        return false;
    }

    void draw()
    {
        m_animation->draw(m_player->x(), m_player->y());
        m_player->show_life();
        m_player->show_sanity();
        m_player->show_inventory();
    }

    void update(unsigned long elapsed)
    {
        if (m_left)
        {
            m_player->set_moviment(-1.0, 0.0);
            m_player->set_direction(Player::LEFT);
            m_player->report_event(Player::MOVED);
        } else if (m_right)
        {
            m_player->set_moviment(0.0, 0.0);
            m_player->set_direction(Player::RIGHT);
            m_player->report_event(Player::MOVED);
        }
        if (m_top)
        {
            m_player->set_moviment(0.0, -1.0);
            m_player->set_direction(Player::UP);
            m_player->report_event(Player::MOVED);
        }else if (m_down)
        {
            m_player->set_moviment(0.0, 1.0);
            m_player->set_direction(Player::DOWN);
            m_player->report_event(Player::MOVED);
        }

        Player::Direction dir = m_player->direction();
        int row = dir;
        m_animation->set_row(row);
        m_animation->update(elapsed);
    }

private:
    Player *m_player;
    unique_ptr<Animation> m_animation;
    int m_left, m_right, m_top, m_down;
};

class Running : public SpriteState
{
public:
    Running(Player *player, Map * current_map, bool key)
        : m_player(player), m_animation(
        new Animation("res/sprites/running.png", 0, 0, 70, 70, 8, 60, true)),
        m_left(0), m_right(0), m_top(0), m_down(0), m_last(0), 
        current_map(current_map), m_key(key)
    {
    }

    ~Running() {}

    const double speed = 350.0;

    void enter(int from)
    {
        m_player->set_dimensions(m_animation->w(), m_animation->h());

        Player::Direction dir = m_player->direction();

        m_right = dir == Player::RIGHT ? 1 : 0;
        m_left = dir == Player::LEFT ? 1 : 0;
        m_top = dir == Player::UP ? 1 : 0;
        m_down = dir == Player::DOWN ? 1 : 0;
        m_last = 0;

        if (from == Player::IDLE)
        {
            auto moviment = m_player->moviment();
            double x = moviment.first * speed;
            double y = moviment.second * speed;
            m_player->set_moviment(x, y);
        }
    }

    void leave(int)
    {
    }

    void draw()
    {
        m_animation->draw(m_player->x(), m_player->y());
        m_player->show_life();
        m_player->show_sanity();
        m_player->show_inventory();
    }

    bool on_event(const KeyboardEvent& event)
    {
        switch (event.state())
        {
        case KeyboardEvent::PRESSED:
            switch (event.key())
            {
            case KeyboardEvent::LEFT:
                m_left = 1;
                return true;

            case KeyboardEvent::RIGHT:
                m_right = 1;
                return true;

            case KeyboardEvent::UP:
                m_top = 1;
                return true;

            case KeyboardEvent::DOWN:
                m_down = 1;
                return true;

            default:
                break;
            }
            break;

        case KeyboardEvent::RELEASED:
            switch (event.key())
            {
            case KeyboardEvent::LEFT:
                m_left = 0;
                return true;

            case KeyboardEvent::RIGHT:
                m_right = 0;
                return true;

            case KeyboardEvent::UP:
                m_top = 0;
                return true;

            case KeyboardEvent::DOWN:
                m_down = 0;
                return true;

            default:
                break;
            }
            break;
        }

        return false;
    }

    void update(unsigned long elapsed)
    {
        m_player->show_life();
        if (m_left)
        {
            m_player->set_direction(Player::LEFT);
            m_player->report_event(Player::MOVED);
        }else if (m_right)
        {
            m_player->set_direction(Player::RIGHT);
            m_player->report_event(Player::MOVED);
        }
        if (m_top)
        {
            m_player->set_direction(Player::UP);
            m_player->report_event(Player::MOVED);
        } else if (m_down)
        {
            m_player->set_direction(Player::DOWN);
            m_player->report_event(Player::MOVED);
        }

        if(!m_top && !m_down && !m_right && !m_left)
        {
            m_player->report_event(Player::STOPPED);
        }
        else
        {
            m_player->set_moviment(speed*(m_right - m_left), speed*(m_down - m_top));
        }

        

        Player::Direction dir = m_player->direction();
        int row = dir;
        m_animation->set_row(row);

        if (not m_last)
        {
            m_last = elapsed;
        }

        auto moviment = m_player->moviment();
        unsigned long delta = elapsed - m_last;
        double x = m_player->x() + (moviment.first * delta)/1000.0;
        double y = m_player->y() + (moviment.second * delta)/1000.0;

        /*limite da sala */
        Environment *env = Environment::get_instance();

        if (x + m_player->w() > env->canvas->w())
        {
            x = env->canvas->w() - m_player->w();
        }

        if ((x >= env->canvas->w() - m_player->w() and moviment.first > 0) or 
            (x <= 0 and moviment.first < 0))
        {
            x -= (moviment.first * delta)/1000.0;
        }

        if (y + m_player->h() > env->canvas->h())
        {
            y = env->canvas->h() - m_player->h();
        }

        if ((y >= env->canvas->h() - m_player->h() and moviment.second > 0) or 
            (y <= 0 and moviment.second < 0))
        {
            y -= (moviment.second * delta)/1000.0;
        }

        m_player->set_x(x);
        m_player->set_y(y);

        m_last = elapsed;
        m_animation->update(elapsed);

    }

private:
    Player *m_player;
    unique_ptr<Animation> m_animation;
    short m_left, m_right, m_top, m_down;
    unsigned long m_last;
    Map * current_map;
    bool m_key;
};

Player::Player(Object *parent, const string& id, Map *current_map)
    : Sprite(parent, id), m_impl(new Player::Impl(this, current_map, m_key)),
     m_key(false)
{
    add_state(IDLE, new Idle(this));
    add_state(RUNNING, new Running(this, current_map, m_key));

    add_transition(MOVED, IDLE, RUNNING);
    add_transition(STOPPED, RUNNING, IDLE);
    change_state(IDLE, NONE);

    Environment *env = Environment::get_instance();
    env->events_manager->register_listener(this);
}

Player::~Player()
{
    Environment *env = Environment::get_instance();
    env->events_manager->unregister_listener(this);
}

Player::Direction
Player::direction() const
{
    return m_impl->direction();
}

void
Player::set_direction(Direction direction)
{
    m_impl->set_direction(direction);
}

const pair<double, double>&
Player::moviment() const
{
    return m_impl->moviment();
}

void
Player::set_moviment(double xaxis, double yaxis)
{
    m_impl->set_moviment(xaxis, yaxis);
}

void
Player::set_current(Room * nova, int posx, int posy)
{
    m_impl->set_current(nova, posx, posy);
}

void
Player::set_strength(double strength)
{
    m_impl->set_strength(strength);
}

void
Player::set_life(double life)
{
    m_impl->set_life(life);
}

void
Player::set_sanity(double sanity)
{
    m_impl->set_sanity(sanity);
}


double
Player::strength()
{
    return m_impl->strength();
}

double
Player::life()
{
    return m_impl->life();
}

double
Player::sanity()
{
    return m_impl->sanity();
}

void
Player::get_key()
{
    m_impl->get_key();
}

void
Player::set_key(bool input)
{
    m_impl->set_key(input);
}

bool
Player::has_key()
{
    return m_impl->has_key();
}

void
Player::show_life()
{
     m_impl->show_life();
}
void
Player::show_sanity()
{
    m_impl->show_sanity();
}
void
Player::show_inventory()
{
    m_impl->show_inventory();
}