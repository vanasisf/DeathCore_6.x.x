/*
 * Copyright (C) 2013-2015 DeathCore <http://www.noffearrdeathproject.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "terrace_of_the_endless_spring.h"

enum Yells
{
};

enum Spells
{
};

enum Events
{
};

class boss_tsulong : public CreatureScript
{
    public:
        boss_tsulong() : CreatureScript("boss_tsulong") { }

        struct boss_tsulongAI : public BossAI
        {
            boss_tsulongAI(Creature* creature) : BossAI(creature, DATA_TSULONG)
            {
            }

            void Reset()
            {
            }

            void EnterCombat(Unit* /*who*/)
            {
            }

            void JustDied(Unit* /*killer*/)
            {
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                /*
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }
                */

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_tsulongAI(creature);
        }
};

void AddSC_boss_tsulong()
{
    new boss_tsulong();
}
