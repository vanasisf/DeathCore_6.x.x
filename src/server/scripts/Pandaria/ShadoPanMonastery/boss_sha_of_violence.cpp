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
 
/* # Script de Tydrheal : Sha Of Violence # */

/* Notes : Tester -- voir spells -- Ajouter SoundID
Sha of violence : Script 95% reste seulement � v�rifier les spells		


UPDATE creature_template SET ScriptName = 'boss_sha_of_violence' WHERE entry = 56719;

INSERT INTO creature_text (entry, groupid, id, text, type, language, probability, emote, duration, sound, comment) VALUES
(56719, 0, 0, "Je ne me laisserai plus enfermer. Ces Pandashan n�ont pas pu m�arr�ter et vous en serez incapables vous aussi !", 14, 0, 100, 0, 0, ???????, "Sha Of Violence - EnterCombat"),
(56719, 1, 0, "Tant que la violence sera tapie dans vos c�urs� je� reviendrai�", 14, 0, 100, 0, 0, ???????, "Sha Of Violence - Death"),
(56719, 2, 0, " Encore une mort violente�", 14, 0, 100, 0, 0, ???????, "Sha Of Violence - Slay"),


*/

#include "ScriptPCH.h"
#include "shadopan_monastery.h"

enum Spells
{
	/* Sha of violence */
	SPELL_DISORIENTING_SMASH = 106872,
	SPELL_SMOKE_BLADES = 106827,
	SPELL_PARTING_SMOKE = 127576,
	SPELL_SHA_SPIKE = 106871,
	SPELL_ENRAGE = 38166
};

enum Events
{
	EVENT_DISORIENTING_SMASH,
	EVENT_SMOKE_BLADES,
	EVENT_PARTING_SMOKE,
	EVENT_SHA_SPIKE_1,
	EVENT_SHA_SPIKE_2,
	EVENT_ENRAGE
};

enum Actions
{
	ACTION_BOSS_SHA_OF_VIOLENCE_RESET,
	ACTION_BOSS_SHA_OF_VIOLENCE_DIED
};

enum Texts
{
	SAY_COMBAT = 0,
	SAY_DEATH = 1,
	SAY_SLAY = 2
};


class boss_sha_of_violence : public CreatureScript
{
public:
	boss_sha_of_violence() : CreatureScript("boss_sha_of_violence") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_sha_of_violenceAI(creature);
	}

	struct boss_sha_of_violenceAI : public ScriptedAI
	{
		boss_sha_of_violenceAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		bool checkShaOfViolenceAlive;
		bool checkEnrage;
		
		void Reset()
		{
			checkShaOfViolenceAlive = true;
			checkEnrage = false;
			checkShaOfViolenceAlive = me->isAlive();

			if(Aura* aura = me->GetAura(SPELL_ENRAGE))
				me->RemoveAura(aura);
			
			events.Reset();

			if (instance)
			{
				instance->SetBossState(DATA_BOSS_SHA_OF_VIOLENCE, NOT_STARTED);
				me->AI()->DoAction(ACTION_BOSS_SHA_OF_VIOLENCE_RESET);
			}
			
		}

		void DoAction(int32 action)
        {
            switch (action)
            {
				case ACTION_BOSS_SHA_OF_VIOLENCE_RESET:
					checkShaOfViolenceAlive = true;
					break;
				case ACTION_BOSS_SHA_OF_VIOLENCE_DIED:
					checkShaOfViolenceAlive = false;
					break;
				
			}
        }

		void JustDied(Unit *pWho)
		{
			if (instance)
			{
				me->AI()->DoAction(ACTION_BOSS_SHA_OF_VIOLENCE_DIED);

				if (!checkShaOfViolenceAlive)
					instance->SetBossState(DATA_BOSS_SHA_OF_VIOLENCE, DONE);

			}

			Talk(SAY_DEATH);
			
		}

		void KilledUnit(Unit *pWho)
		{
			Talk(SAY_SLAY);			
		}
		
		void EnterEvadeMode()
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_SHA_OF_VIOLENCE, FAIL);			
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_SHA_OF_VIOLENCE, IN_PROGRESS);

			me->SetInCombatWithZone();
			Talk(SAY_COMBAT);
			events.ScheduleEvent(EVENT_SHA_SPIKE_2, 6*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_SMOKE_BLADES, 8*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_DISORIENTING_SMASH, 13*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_SHA_SPIKE_1, 14*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 diff)
		{
			if(!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_DISORIENTING_SMASH:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_DISORIENTING_SMASH);
								}
							events.ScheduleEvent(EVENT_DISORIENTING_SMASH, 15*IN_MILLISECONDS);
							break;
						
						case EVENT_SMOKE_BLADES:
							DoCast(me, SPELL_SMOKE_BLADES);
							events.ScheduleEvent(EVENT_PARTING_SMOKE, 2500);
							break;
							
						case EVENT_PARTING_SMOKE:
							DoCast(me, SPELL_PARTING_SMOKE);
							events.ScheduleEvent(EVENT_SMOKE_BLADES, 12500);
							break;
							
						case EVENT_SHA_SPIKE_1:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_SHA_SPIKE);
								}
							events.ScheduleEvent(EVENT_SHA_SPIKE_1, 15*IN_MILLISECONDS);
							break;
							
						case EVENT_SHA_SPIKE_2:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_SHA_SPIKE);
								}
							events.ScheduleEvent(EVENT_SHA_SPIKE_2, 15*IN_MILLISECONDS);
							break;
							
						if (!checkEnrage)
						{	case EVENT_ENRAGE:
								DoCast(me, SPELL_ENRAGE);
								checkEnrage = true;
								break;
						}
							
						default:
							break;
					}
				}
			}
			
			if(me->HealthBelowPct(25) && !checkEnrage)
			{
				events.ScheduleEvent(EVENT_ENRAGE, 0);
			}
			

			DoMeleeAttackIfReady();
		}
	};
};


void AddSC_boss_sha_of_violence()
{
	new boss_sha_of_violence();
}